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

#include "ChainImporter.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/extensions/LocalNodeStateFileStorage.h"
#include "bitxorcore/extensions/LocalNodeStateRef.h"
#include "bitxorcore/extensions/GenesisBlockLoader.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/io/FileQueue.h"
#include "bitxorcore/local/HostUtils.h"
#include "bitxorcore/local/recovery/MultiBlockLoader.h"
#include "bitxorcore/local/recovery/RecoveryStorageAdapter.h"
#include "bitxorcore/local/server/FileStateChangeStorage.h"
#include "bitxorcore/local/server/GenesisBlockNotifier.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace local {

	namespace {
		std::unique_ptr<io::PrunableBlockStorage> CreateStagingBlockStorage(
				const config::BitxorCoreDataDirectory& dataDirectory,
				uint32_t fileDatabaseBatchSize) {
			auto stagingDirectory = dataDirectory.spoolDir("block_recover").str();
			config::BitxorCoreDirectory(stagingDirectory).create();
			return std::make_unique<io::FileBlockStorage>(stagingDirectory, fileDatabaseBatchSize, io::FileBlockStorageMode::None);
		}

		std::unique_ptr<subscribers::StateChangeSubscriber> CreateStateChangeSubscriber(
				subscribers::SubscriptionManager& subscriptionManager,
				const cache::BitxorCoreCache& bitxorcoreCache,
				const config::BitxorCoreDataDirectory& dataDirectory) {
			subscriptionManager.addStateChangeSubscriber(CreateFileStateChangeStorage(
					std::make_unique<io::FileQueueWriter>(dataDirectory.spoolDir("state_change").str(), "index_server.dat"),
					[&bitxorcoreCache]() { return bitxorcoreCache.changesStorages(); }));
			return subscriptionManager.createStateChangeSubscriber();
		}

#pragma pack(push, 1)

		struct FinalizationProofHeader : public model::SizePrefixedEntity {
			uint32_t Version;
			model::FinalizationRound Round;
			bitxorcore::Height Height;
			Hash256 Hash;
		};

#pragma pack(pop)

		class DefaultChainImporter final : public ChainImporter {
		public:
			explicit DefaultChainImporter(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper)
					: m_pBootstrapper(std::move(pBootstrapper))
					, m_config(m_pBootstrapper->config())
					, m_dataDirectory(config::BitxorCoreDataDirectoryPreparer::Prepare(m_config.User.DataDirectory))
					, m_bitxorcoreCache({}) // note that sub caches are added in boot
					, m_pBlockStorage(m_pBootstrapper->subscriptionManager().createBlockStorage(m_pBlockChangeSubscriber))
					, m_storage(
							CreateReadOnlyStorageAdapter(*m_pBlockStorage),
							CreateStagingBlockStorage(m_dataDirectory, m_config.Node.FileDatabaseBatchSize))
					, m_pFinalizationSubscriber(m_pBootstrapper->subscriptionManager().createFinalizationSubscriber())
					, m_pStateChangeSubscriber(CreateStateChangeSubscriber(
							m_pBootstrapper->subscriptionManager(),
							m_bitxorcoreCache,
							m_dataDirectory))
					, m_pluginManager(m_pBootstrapper->pluginManager())
			{}

			~DefaultChainImporter() override {
				shutdown();
			}

		public:
			void boot() {
				BITXORCORE_LOG(info) << "registering system plugins";
				m_pluginModules = LoadAllPlugins(*m_pBootstrapper);

				BITXORCORE_LOG(debug) << "initializing cache";
				m_bitxorcoreCache = m_pluginManager.createCache();

				utils::StackLogger stackLogger("running chain importer operations", utils::LogLevel::info);
				import();
			}

		private:
			void import() {
				if (extensions::HasSerializedState(m_dataDirectory.dir("state")))
					BITXORCORE_THROW_INVALID_ARGUMENT("import failed because serialized state is detected");

				// genesis notification
				GenesisBlockNotifier notifier(m_config.Blockchain, m_bitxorcoreCache, m_storage, m_pluginManager);

				if (m_pBlockChangeSubscriber)
					notifier.raise(*m_pBlockChangeSubscriber);

				notifier.raise(*m_pFinalizationSubscriber);
				notifier.raise(*m_pStateChangeSubscriber);

				// genesis loader
				{
					auto cacheDelta = m_bitxorcoreCache.createDelta();
					extensions::GenesisBlockLoader loader(cacheDelta, m_pluginManager, m_pluginManager.createObserver());
					loader.executeAndCommit(stateRef(), extensions::StateHashVerification::Enabled);
					stateRef().Score += model::ChainScore(1); // set chain score to 1 after processing genesis
				}

				// load rest of chain
				loadAllBlocks([this](auto&& loadedBlockStatus) {
					if (m_pBlockChangeSubscriber)
						m_pBlockChangeSubscriber->notifyBlock(loadedBlockStatus.BlockElement);

					m_pStateChangeSubscriber->notifyScoreChange(loadedBlockStatus.ChainScore);
					m_pStateChangeSubscriber->notifyStateChange(loadedBlockStatus.StateChangeInfo);
				});

				// fix up index
				auto stateChangeDir = m_dataDirectory.spoolDir("state_change");
				std::filesystem::copy(stateChangeDir.file("index_server.dat"), stateChangeDir.file("index.dat"));

				// process proofs
				generateFinalizationNotifications();
			}

			void loadAllBlocks(const consumer<LoadedBlockStatus&&>& statusConsumer) {
				// disable load optimizations (loading from the saved state is optimization enough) in order to prevent
				// discontinuities in block analysis (e.g. statistic cache expects consecutive blocks)
				auto observerFactory = [&pluginManager = m_pluginManager](const auto&) { return pluginManager.createObserver(); };
				auto partialScore = LoadBlockchain(observerFactory, m_pluginManager, stateRef(), Height(2), statusConsumer);
				m_score += partialScore;
			}

			void generateFinalizationNotifications() {
				io::FileDatabase proofFileDatabase(m_dataDirectory.rootDir(), { m_config.Node.FileDatabaseBatchSize, ".proof" });
				for (uint64_t id = 2; proofFileDatabase.contains(id); ++id) {
					auto pProofStream = proofFileDatabase.inputStream(id);

					FinalizationProofHeader proofHeader;
					pProofStream->read({ reinterpret_cast<uint8_t*>(&proofHeader), sizeof(FinalizationProofHeader) });
					m_pFinalizationSubscriber->notifyFinalizedBlock(proofHeader.Round, proofHeader.Height, proofHeader.Hash);
				}
			}

		public:
			void shutdown() override {
				utils::StackLogger stackLogger("shutting down chain importer", utils::LogLevel::info);

				m_pBootstrapper->pool().shutdown();
				saveStateToDisk();
			}

		private:
			void saveStateToDisk() {
				constexpr auto SaveStateToDirectoryWithCheckpointing = extensions::SaveStateToDirectoryWithCheckpointing;
				SaveStateToDirectoryWithCheckpointing(m_dataDirectory, m_config.Node, m_bitxorcoreCache, m_score.get());
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

			plugins::PluginManager& m_pluginManager;
		};
	}

	std::unique_ptr<ChainImporter> CreateChainImporter(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper) {
		return CreateAndBootHost<DefaultChainImporter>(std::move(pBootstrapper));
	}
}}
