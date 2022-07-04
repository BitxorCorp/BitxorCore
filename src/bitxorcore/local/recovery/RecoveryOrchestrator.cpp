/**
*** Copyright (c) 2016-2019, Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp.
*** Copyright (c) 2020-2021, Jaguar0625, gimre, BloodyRookie.
*** Copyright (c) 2022-present, Kriptxor Corp, Microsula S.A.
*** All rights reserved.
***
*** This file is part of BitxorCore.
***
*** BitxorCore is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** BitxorCore is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with BitxorCore. If not, see <http://www.gnu.org/licenses/>.
**/

#include "RecoveryOrchestrator.h"
#include "BitxorCoreSystemState.h"
#include "MultiBlockLoader.h"
#include "RecoveryStorageAdapter.h"
#include "RepairImportance.h"
#include "RepairSpooling.h"
#include "RepairState.h"
#include "StateChangeRepairingSubscriber.h"
#include "StateRecoveryMode.h"
#include "StorageStart.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache_core/BlockStatisticCache.h"
#include "bitxorcore/chain/BlockExecutor.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/extensions/LocalNodeStateRef.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/io/FilesystemUtils.h"
#include "bitxorcore/io/MoveBlockFiles.h"
#include "bitxorcore/local/HostUtils.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "bitxorcore/subscribers/BlockChangeReader.h"
#include "bitxorcore/subscribers/BrokerMessageReaders.h"
#include "bitxorcore/subscribers/FinalizationReader.h"
#include "bitxorcore/subscribers/TransactionStatusReader.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace local {

	namespace {
		// region DualStateChangeSubscriber

		class DualStateChangeSubscriber final : public subscribers::StateChangeSubscriber {
		public:
			DualStateChangeSubscriber(subscribers::StateChangeSubscriber& subscriber1, subscribers::StateChangeSubscriber& subscriber2)
					: m_subscriber1(subscriber1)
					, m_subscriber2(subscriber2)
			{}

		public:
			void notifyScoreChange(const model::ChainScore& chainScore) override {
				m_subscriber1.notifyScoreChange(chainScore);
				m_subscriber2.notifyScoreChange(chainScore);
			}

			void notifyStateChange(const subscribers::StateChangeInfo& stateChangeInfo) override {
				m_subscriber1.notifyStateChange(stateChangeInfo);
				m_subscriber2.notifyStateChange(stateChangeInfo);
			}

		private:
			subscribers::StateChangeSubscriber& m_subscriber1;
			subscribers::StateChangeSubscriber& m_subscriber2;
		};

		// endregion

		std::unique_ptr<io::PrunableBlockStorage> CreateStagingBlockStorage(
				const config::BitxorCoreDataDirectory& dataDirectory,
				uint32_t fileDatabaseBatchSize) {
			auto stagingDirectory = dataDirectory.spoolDir("block_recover").str();
			config::BitxorCoreDirectory(stagingDirectory).create();
			return std::make_unique<io::FileBlockStorage>(stagingDirectory, fileDatabaseBatchSize, io::FileBlockStorageMode::None);
		}

		void MoveSupplementalDataFiles(const config::BitxorCoreDataDirectory& dataDirectory) {
			if (!std::filesystem::exists(dataDirectory.dir("state.tmp").path()))
				return;

			extensions::LocalNodeStateSerializer serializer(dataDirectory.dir("state.tmp"));
			serializer.moveTo(dataDirectory.dir("state"));
		}

		Height MoveBlockFiles(
				const config::BitxorCoreDirectory& stagingDirectory,
				io::BlockStorage& destinationStorage,
				uint32_t fileDatabaseBatchSize) {
			io::FileBlockStorage staging(stagingDirectory.str(), fileDatabaseBatchSize);
			if (Height(0) == staging.chainHeight())
				return Height(0);

			// mind that startHeight will be > 1, so there is no additional check
			auto startHeight = FindStartHeight(staging);
			BITXORCORE_LOG(debug) << "moving blocks: " << startHeight << "-" << staging.chainHeight();
			io::MoveBlockFiles(staging, destinationStorage, startHeight);
			return startHeight;
		}

		class DefaultRecoveryOrchestrator final : public RecoveryOrchestrator {
		public:
			explicit DefaultRecoveryOrchestrator(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper)
					: m_pBootstrapper(std::move(pBootstrapper))
					, m_config(m_pBootstrapper->config())
					, m_dataDirectory(config::BitxorCoreDataDirectoryPreparer::Prepare(m_config.User.DataDirectory))
					, m_bitxorcoreCache({}) // note that sub caches are added in boot
					, m_pBlockStorage(m_pBootstrapper->subscriptionManager().createBlockStorage(m_pBlockChangeSubscriber))
					, m_storage(
							CreateReadOnlyStorageAdapter(*m_pBlockStorage),
							CreateStagingBlockStorage(m_dataDirectory, m_config.Node.FileDatabaseBatchSize))
					, m_pFinalizationSubscriber(m_pBootstrapper->subscriptionManager().createFinalizationSubscriber())
					, m_pStateChangeSubscriber(m_pBootstrapper->subscriptionManager().createStateChangeSubscriber())
					, m_pTransactionStatusSubscriber(m_pBootstrapper->subscriptionManager().createTransactionStatusSubscriber())
					, m_pluginManager(m_pBootstrapper->pluginManager())
					, m_stateSavingRequired(true)
			{}

			~DefaultRecoveryOrchestrator() override {
				shutdown();
			}

		public:
			void boot() {
				BITXORCORE_LOG(info) << "registering system plugins";
				m_pluginModules = LoadAllPlugins(*m_pBootstrapper);

				BITXORCORE_LOG(debug) << "initializing cache";
				m_bitxorcoreCache = m_pluginManager.createCache();

				utils::StackLogger stackLogger("running recovery operations", utils::LogLevel::info);
				recover();
			}

		private:
			void recover() {
				auto systemState = BitxorCoreSystemState(m_dataDirectory);

				BITXORCORE_LOG(info) << "repairing spooling, commit step " << utils::to_underlying_type(systemState.commitStep());
				RepairSpooling(m_dataDirectory, systemState.commitStep());

				BITXORCORE_LOG(info) << "repairing importance";
				RepairImportance(m_dataDirectory, systemState.commitStep());

				BITXORCORE_LOG(info) << "repairing messages";
				repairSubscribers();

				BITXORCORE_LOG(info) << "loading state";
				auto heights = extensions::LoadStateFromDirectory(m_dataDirectory.dir("state"), stateRef(), m_pluginManager);
				auto stateRecoveryMode = CalculateStateRecoveryMode(m_config.Node, heights);
				if (StateRecoveryMode::Repair == stateRecoveryMode)
					repairStateFromStorage(heights);
				else if (StateRecoveryMode::Reseed == stateRecoveryMode)
					BITXORCORE_THROW_INVALID_ARGUMENT("reseed operation detected - please use the importer tool");

				BITXORCORE_LOG(info) << "loaded blockchain (height = " << heights.Storage << ", score = " << m_score.get() << ")";

				BITXORCORE_LOG(info) << "repairing state";
				repairState(systemState.commitStep());

				BITXORCORE_LOG(info) << "finalizing";
				systemState.reset();
			}

			void repairSubscribers() {
				// due to behavior of SubscriptionManager, block change subscriber is only subscriber that can be nullptr
				if (m_pBlockChangeSubscriber)
					processMessages("block_change", *m_pBlockChangeSubscriber, subscribers::ReadNextBlockChange);

				processMessages("finalization", *m_pFinalizationSubscriber, subscribers::ReadNextFinalization);
				processMessages("transaction_status", *m_pTransactionStatusSubscriber, subscribers::ReadNextTransactionStatus);
			}

			template<typename TSubscriber, typename TMessageReader>
			void processMessages(const std::string& queueName, TSubscriber& subscriber, TMessageReader readNextMessage) {
				subscribers::ReadAll(
						{ m_dataDirectory.spoolDir(queueName).str(), "index_broker_r.dat", "index.dat" },
						subscriber,
						readNextMessage);
			}

			void repairStateFromStorage(const extensions::StateHeights& heights) {
				// disable load optimizations (loading from the saved state is optimization enough) in order to prevent
				// discontinuities in block analysis (e.g. statistic cache expects consecutive blocks)
				BITXORCORE_LOG(info) << "loading state - block loading required";
				auto observerFactory = [&pluginManager = m_pluginManager](const auto&) { return pluginManager.createObserver(); };
				auto partialScore = LoadBlockchain(observerFactory, m_pluginManager, stateRef(), heights.Cache + Height(1));
				m_score += partialScore;
			}

			void repairState(consumers::CommitOperationStep commitStep) {
				// RepairState always needs to be called in order to recover broker messages
				std::unique_ptr<subscribers::StateChangeSubscriber> pStateChangeRepairSubscriber;
				std::unique_ptr<subscribers::StateChangeSubscriber> pDualStateChangeSubscriber;
				if (stateRef().Config.Node.EnableCacheDatabaseStorage) {
					pStateChangeRepairSubscriber = CreateStateChangeRepairingSubscriber(stateRef().Cache, stateRef().Score);
					pDualStateChangeSubscriber = std::make_unique<DualStateChangeSubscriber>(
							*pStateChangeRepairSubscriber,
							*m_pStateChangeSubscriber);
				}

				auto& repairSubscriber = stateRef().Config.Node.EnableCacheDatabaseStorage
						? *pDualStateChangeSubscriber
						: *m_pStateChangeSubscriber;
				RepairState(m_dataDirectory.spoolDir("state_change"), stateRef().Cache, *m_pStateChangeSubscriber, repairSubscriber);

				if (consumers::CommitOperationStep::State_Written != commitStep) {
					BITXORCORE_LOG(debug) << " - purging state.tmp";
					io::PurgeDirectory(m_dataDirectory.dir("state.tmp").str());
					return;
				}

				BITXORCORE_LOG(debug) << " - moving supplemental data and block files";
				MoveSupplementalDataFiles(m_dataDirectory);
				auto startHeight = MoveBlockFiles(
						m_dataDirectory.spoolDir("block_sync"),
						*m_pBlockStorage,
						m_config.Node.FileDatabaseBatchSize);

				// when verifiable state is enabled, forcibly regenerate all patricia trees because cache changes are coalesced
				if (stateRef().Config.Blockchain.EnableVerifiableState && startHeight > Height(0)) {
					BITXORCORE_LOG(debug) << "- reloading supplemental state";
					extensions::LoadDependentStateFromDirectory(m_dataDirectory.dir("state"), stateRef().Cache);
					reapplyBlocks(startHeight);
				}

				// when cache database storage is enabled and State_Written,
				// cache database and supplemental data are updated by repairState
				m_stateSavingRequired = !stateRef().Config.Node.EnableCacheDatabaseStorage;
			}

			void reapplyBlocks(Height startHeight) {
				auto chainHeight = m_pBlockStorage->chainHeight();
				BITXORCORE_LOG(info) << "reapplying blocks to regenerate patricia trees " << startHeight << "-" << chainHeight;

				auto cacheDelta = stateRef().Cache.createDelta();
				auto observerState = observers::ObserverState(cacheDelta);

				auto readOnlyCache = cacheDelta.toReadOnly();
				auto resolverContext = m_pluginManager.createResolverContext(readOnlyCache);

				observers::NotificationObserverAdapter observer(
						m_pluginManager.createObserver(),
						m_pluginManager.createNotificationPublisher());
				chain::BlockExecutionContext executionContext{ observer, resolverContext, observerState };

				// reapplyBlocks is only called when EnableVerifiableState is true.
				// non rocksdb backed caches (e.g. BlockStatisticCache) are loaded initially from "state" directory
				// but are not updated / reloaded after copying "state.tmp" to "state".
				// of these, only BlockStatisticCache is problematic because it has strict bounds checking.
				// as a workaround, fill it with statistics up to the new chain height.
				FillBlockStatisticCache(cacheDelta.sub<cache::BlockStatisticCache>(), chainHeight);

				BITXORCORE_LOG(debug) << " - rolling back blocks";
				for (auto height = chainHeight; height >= startHeight; height = height - Height(1))
					chain::RollbackBlock(*m_pBlockStorage->loadBlockElement(height), executionContext);

				BITXORCORE_LOG(debug) << " - executing blocks";
				for (auto height = startHeight; height <= chainHeight; height = height + Height(1)) {
					chain::ExecuteBlock(*m_pBlockStorage->loadBlockElement(height), executionContext);
					cacheDelta.calculateStateHash(height); // force recalculation of patricia tree
				}

				stateRef().Cache.commit(chainHeight);
			}

		private:
			static void FillBlockStatisticCache(cache::BlockStatisticCacheDelta& blockStatisticCacheDelta, Height newHeight) {
				auto originalHeight = newHeight;
				while (!blockStatisticCacheDelta.contains(state::BlockStatistic(originalHeight)))
					originalHeight = originalHeight - Height(1);

				// these statistics are placeholders and don't need to reflect the block exactly as long as the height is correct
				for (auto height = originalHeight + Height(1); height <= newHeight; height = height + Height(1))
					blockStatisticCacheDelta.insert(state::BlockStatistic(height));
			}

		public:
			void shutdown() override {
				utils::StackLogger stackLogger("shutting down recovery orchestrator", utils::LogLevel::info);

				m_pBootstrapper->pool().shutdown();
				saveStateToDisk();
			}

		private:
			void saveStateToDisk() {
				if (!m_stateSavingRequired) {
					// just write the commit step file that was deleted during recover
					io::IndexFile commitStepFile(m_dataDirectory.rootDir().file("commit_step.dat"));
					commitStepFile.set(utils::to_underlying_type(consumers::CommitOperationStep::All_Updated));
					return;
				}

				constexpr auto SaveStateToDirectoryWithCheckpointing = extensions::SaveStateToDirectoryWithCheckpointing;
				SaveStateToDirectoryWithCheckpointing(m_dataDirectory, m_config.Node, m_bitxorcoreCache, m_score.get());
			}

		public:
			model::ChainScore score() const override {
				return m_score.get();
			}

		private:
			extensions::LocalNodeStateRef stateRef() {
				return extensions::LocalNodeStateRef(m_config, m_bitxorcoreCache, m_storage, m_score);
			}

		private:
			// make sure modules are unloaded last
			std::vector<plugins::PluginModule> m_pluginModules;
			std::unique_ptr<extensions::ProcessBootstrapper> m_pBootstrapper;

			io::BlockChangeSubscriber* m_pBlockChangeSubscriber;
			const config::BitxorCoreConfiguration& m_config;
			config::BitxorCoreDataDirectory m_dataDirectory;

			cache::BitxorCoreCache m_bitxorcoreCache;
			std::unique_ptr<io::BlockStorage> m_pBlockStorage;
			io::BlockStorageCache m_storage;
			extensions::LocalNodeChainScore m_score;

			std::unique_ptr<subscribers::FinalizationSubscriber> m_pFinalizationSubscriber;
			std::unique_ptr<subscribers::StateChangeSubscriber> m_pStateChangeSubscriber;
			std::unique_ptr<subscribers::TransactionStatusSubscriber> m_pTransactionStatusSubscriber;

			plugins::PluginManager& m_pluginManager;
			bool m_stateSavingRequired;
		};
	}

	std::unique_ptr<RecoveryOrchestrator> CreateRecoveryOrchestrator(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper) {
		return CreateAndBootHost<DefaultRecoveryOrchestrator>(std::move(pBootstrapper));
	}
}}
