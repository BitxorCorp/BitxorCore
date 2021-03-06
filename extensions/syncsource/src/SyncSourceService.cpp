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

#include "SyncSourceService.h"
#include "bitxorcore/cache_tx/MemoryUtCache.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/extensions/ServerHooksUtils.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/handlers/ChainHandlers.h"
#include "bitxorcore/handlers/TransactionHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace syncsource {

	namespace {
		struct HandlersConfiguration {
			uint32_t MaxHashes;
			handlers::PullBlocksHandlerConfiguration BlocksHandlerConfig;

			handlers::BlockRangeHandler PushBlockCallback;
			model::ChainScoreSupplier ChainScoreSupplier;
			handlers::UtRetriever UtRetriever;
		};

		void SetConfig(handlers::PullBlocksHandlerConfiguration& blocksHandlerConfig, const config::NodeConfiguration& nodeConfig) {
			blocksHandlerConfig.MaxBlocks = nodeConfig.MaxBlocksPerSyncAttempt;
			blocksHandlerConfig.MaxResponseBytes = nodeConfig.MaxChainBytesPerSyncAttempt.bytes32();
		}

		void SetConfig(HandlersConfiguration& config, const config::NodeConfiguration& nodeConfig) {
			config.MaxHashes = nodeConfig.MaxHashesPerSyncAttempt;
			SetConfig(config.BlocksHandlerConfig, nodeConfig);
		}

		HandlersConfiguration CreateHandlersConfiguration(const extensions::ServiceState& state) {
			HandlersConfiguration config;
			SetConfig(config, state.config().Node);

			config.PushBlockCallback = extensions::CreateBlockPushEntityCallback(state.hooks());
			config.ChainScoreSupplier = [&chainScore = state.score()]() { return chainScore.get(); };
			config.UtRetriever = [&cache = state.utCache()](auto minDeadline, auto minFeeMultiplier, const auto& shortHashes) {
				return cache.view().unknownTransactions(minDeadline, minFeeMultiplier, shortHashes);
			};

			return config;
		}

		void RegisterAllHandlers(extensions::ServiceState& state) {
			auto& handlers = state.packetHandlers();
			const auto& storage = state.storage();
			const auto& registry = state.pluginManager().transactionRegistry();
			auto config = CreateHandlersConfiguration(state);

			handlers::RegisterPushBlockHandler(handlers, registry, config.PushBlockCallback);
			handlers::RegisterPullBlockHandler(handlers, storage);

			handlers::RegisterChainStatisticsHandler(
					handlers,
					storage,
					config.ChainScoreSupplier,
					extensions::CreateLocalFinalizedHeightSupplier(state));
			handlers::RegisterBlockHashesHandler(handlers, storage, config.MaxHashes);
			handlers::RegisterPullBlocksHandler(handlers, storage, config.BlocksHandlerConfig);

			handlers::RegisterPullTransactionsHandler(handlers, config.UtRetriever);
		}

		class SyncSourceServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "SyncSource", extensions::ServiceRegistrarPhase::Post_Range_Consumers };
			}

			void registerServiceCounters(extensions::ServiceLocator&) override {
				// no additional counters
			}

			void registerServices(extensions::ServiceLocator&, extensions::ServiceState& state) override {
				// add handlers
				RegisterAllHandlers(state);
			}
		};
	}

	DECLARE_SERVICE_REGISTRAR(SyncSource)() {
		return std::make_unique<SyncSourceServiceRegistrar>();
	}
}}
