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

#include "Broker.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/io/FileQueue.h"
#include "bitxorcore/local/HostUtils.h"
#include "bitxorcore/subscribers/BlockChangeReader.h"
#include "bitxorcore/subscribers/BrokerMessageReaders.h"
#include "bitxorcore/subscribers/FinalizationReader.h"
#include "bitxorcore/subscribers/PtChangeReader.h"
#include "bitxorcore/subscribers/StateChangeReader.h"
#include "bitxorcore/subscribers/TransactionStatusReader.h"
#include "bitxorcore/subscribers/UtChangeReader.h"
#include "bitxorcore/thread/Scheduler.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace local {

	namespace {
		class DefaultBroker final : public Broker {
		public:
			explicit DefaultBroker(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper)
					: m_pBootstrapper(std::move(pBootstrapper))
					, m_dataDirectory(config::BitxorCoreDataDirectoryPreparer::Prepare(m_pBootstrapper->config().User.DataDirectory))
					, m_bitxorcoreCache({}) // note that sub caches are added in boot
					, m_pBlockChangeSubscriber(m_pBootstrapper->subscriptionManager().createBlockChangeSubscriber())
					, m_pUtChangeSubscriber(m_pBootstrapper->subscriptionManager().createUtChangeSubscriber())
					, m_pPtChangeSubscriber(m_pBootstrapper->subscriptionManager().createPtChangeSubscriber())
					, m_pFinalizationSubscriber(m_pBootstrapper->subscriptionManager().createFinalizationSubscriber())
					, m_pStateChangeSubscriber(m_pBootstrapper->subscriptionManager().createStateChangeSubscriber())
					, m_pTransactionStatusSubscriber(m_pBootstrapper->subscriptionManager().createTransactionStatusSubscriber())
					, m_pluginManager(m_pBootstrapper->pluginManager())
			{}

			~DefaultBroker() override {
				shutdown();
			}

		public:
			void boot() {
				BITXORCORE_LOG(info) << "registering system plugins";
				m_pluginModules = LoadAllPlugins(*m_pBootstrapper);

				BITXORCORE_LOG(debug) << "initializing cache";
				m_bitxorcoreCache = m_pluginManager.createCache();

				utils::StackLogger stackLogger("booting broker", utils::LogLevel::info);
				startIngestion();
			}

		public:
			void shutdown() override {
				utils::StackLogger stackLogger("shutting down broker", utils::LogLevel::info);

				m_pBootstrapper->pool().shutdown();
			}

		private:
			void startIngestion() {
				using namespace bitxorcore::subscribers;

				auto pServiceGroup = m_pBootstrapper->pool().pushServiceGroup("scheduler");
				auto pScheduler = pServiceGroup->pushService(thread::CreateScheduler);
				pScheduler->addTask(createIngestionTask("block_change", *m_pBlockChangeSubscriber, ReadNextBlockChange));
				pScheduler->addTask(createIngestionTask("unconfirmed_transactions_change", *m_pUtChangeSubscriber, ReadNextUtChange));
				pScheduler->addTask(createIngestionTask("partial_transactions_change", *m_pPtChangeSubscriber, ReadNextPtChange));
				pScheduler->addTask(createIngestionTask("finalization", *m_pFinalizationSubscriber, ReadNextFinalization));
				pScheduler->addTask(createIngestionTask("state_change", *m_pStateChangeSubscriber, [&bitxorcoreCache = m_bitxorcoreCache](
						auto& inputStream,
						auto& subscriber) {
					return ReadNextStateChange(inputStream, bitxorcoreCache.changesStorages(), subscriber);
				}));
				pScheduler->addTask(createIngestionTask("transaction_status", *m_pTransactionStatusSubscriber, ReadNextTransactionStatus));
			}

			template<typename TSubscriber, typename TMessageReader>
			thread::Task createIngestionTask(const std::string& queueName, TSubscriber& subscriber, TMessageReader readNextMessage) {
				thread::Task task;
				task.StartDelay = utils::TimeSpan::FromMilliseconds(100);
				task.NextDelay = thread::CreateUniformDelayGenerator(utils::TimeSpan::FromMilliseconds(500));
				task.Name = queueName;

				auto queuePath = m_dataDirectory.spoolDir(queueName).str();
				task.Callback = [&subscriber, readNextMessage, queuePath]() {
					subscribers::ReadAll({ queuePath, "index_broker_r.dat", "index.dat" }, subscriber, readNextMessage);
					return thread::make_ready_future(thread::TaskResult::Continue);
				};

				return task;
			}

		private:
			// make sure modules are unloaded last
			std::vector<plugins::PluginModule> m_pluginModules;
			std::unique_ptr<extensions::ProcessBootstrapper> m_pBootstrapper;

			config::BitxorCoreDataDirectory m_dataDirectory;

			cache::BitxorCoreCache m_bitxorcoreCache;

			std::unique_ptr<io::BlockChangeSubscriber> m_pBlockChangeSubscriber;
			std::unique_ptr<cache::UtChangeSubscriber> m_pUtChangeSubscriber;
			std::unique_ptr<cache::PtChangeSubscriber> m_pPtChangeSubscriber;
			std::unique_ptr<subscribers::FinalizationSubscriber> m_pFinalizationSubscriber;
			std::unique_ptr<subscribers::StateChangeSubscriber> m_pStateChangeSubscriber;
			std::unique_ptr<subscribers::TransactionStatusSubscriber> m_pTransactionStatusSubscriber;

			plugins::PluginManager& m_pluginManager;
		};
	}

	std::unique_ptr<Broker> CreateBroker(std::unique_ptr<extensions::ProcessBootstrapper>&& pBootstrapper) {
		return CreateAndBootHost<DefaultBroker>(std::move(pBootstrapper));
	}
}}
