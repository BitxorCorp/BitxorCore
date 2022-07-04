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

#include "PtService.h"
#include "PtBootstrapperService.h"
#include "partialtransaction/src/api/RemotePtApi.h"
#include "partialtransaction/src/chain/PtSynchronizer.h"
#include "bitxorcore/cache_tx/MemoryPtCache.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/extensions/NetworkUtils.h"
#include "bitxorcore/extensions/PeersConnectionTasks.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/extensions/SynchronizerTaskCallbacks.h"
#include "bitxorcore/net/PacketWriters.h"
#include "bitxorcore/thread/MultiServicePool.h"

namespace bitxorcore { namespace partialtransaction {

	namespace {
		constexpr auto Service_Name = "pt.writers";
		constexpr auto Service_Id = ionet::ServiceIdentifier(0x50415254);

		thread::Task CreateConnectPeersTask(extensions::ServiceState& state, net::PacketWriters& packetWriters) {
			auto settings = extensions::CreateOutgoingSelectorSettings(state, Service_Id, ionet::NodeRoles::Api);
			auto task = extensions::CreateConnectPeersTask(settings, packetWriters);
			task.Name += " for service Pt";
			return task;
		}

		thread::Task CreatePullPtTask(
				extensions::ServiceLocator& locator,
				const extensions::ServiceState& state,
				net::PacketWriters& packetWriters) {
			const auto& ptCache = GetMemoryPtCache(locator);
			const auto& serverHooks = GetPtServerHooks(locator);
			auto ptSynchronizer = chain::CreatePtSynchronizer(
					state.timeSupplier(),
					[&ptCache]() { return ptCache.view().shortHashPairs(); },
					serverHooks.cosignedTransactionInfosConsumer(),
					extensions::CreateShouldProcessTransactionsPredicate(state));

			thread::Task task;
			task.Name = "pull partial transactions task";
			task.Callback = CreateChainSyncAwareSynchronizerTaskCallback(
					std::move(ptSynchronizer),
					api::CreateRemotePtApi,
					packetWriters,
					state,
					task.Name);
			return task;
		}

		class PtServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "Pt", extensions::ServiceRegistrarPhase::Post_Extended_Range_Consumers };
			}

			void registerServiceCounters(extensions::ServiceLocator& locator) override {
				locator.registerServiceCounter<net::PacketWriters>(Service_Name, "PT WRITERS", [](const auto& writers) {
					return writers.numActiveWriters();
				});
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				auto connectionSettings = extensions::GetConnectionSettings(state.config());
				auto pServiceGroup = state.pool().pushServiceGroup("partial");
				auto pWriters = pServiceGroup->pushService(net::CreatePacketWriters, locator.keys().caPublicKey(), connectionSettings);

				locator.registerService(Service_Name, pWriters);
				state.packetIoPickers().insert(*pWriters, ionet::NodeRoles::Api);

				// add sinks
				state.hooks().addBannedNodeIdentitySink(extensions::CreateCloseConnectionSink(*pWriters));

				// add tasks
				state.tasks().push_back(CreateConnectPeersTask(state, *pWriters));
				state.tasks().push_back(CreatePullPtTask(locator, state, *pWriters));
			}
		};
	}

	DECLARE_SERVICE_REGISTRAR(Pt)() {
		return std::make_unique<PtServiceRegistrar>();
	}
}}
