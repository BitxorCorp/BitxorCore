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

#include "NetworkPacketReadersService.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/extensions/NetworkUtils.h"
#include "bitxorcore/extensions/PeersConnectionTasks.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/thread/MultiServicePool.h"

namespace bitxorcore { namespace packetserver {

	namespace {
		constexpr auto Service_Name = "readers";
		constexpr auto Service_Id = ionet::ServiceIdentifier(0x52454144);

		thread::Task CreateAgePeersTask(extensions::ServiceState& state, net::ConnectionContainer& connectionContainer) {
			auto settings = extensions::CreateIncomingSelectorSettings(state, Service_Id);
			auto task = extensions::CreateAgePeersTask(settings, connectionContainer);
			task.Name += " for service Readers";
			return task;
		}

		class NetworkPacketReadersServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "NetworkPacketReaders", extensions::ServiceRegistrarPhase::Post_Packet_Handlers };
			}

			void registerServiceCounters(extensions::ServiceLocator& locator) override {
				locator.registerServiceCounter<net::PacketReaders>(Service_Name, "READERS", [](const auto& writers) {
					return writers.numActiveReaders();
				});
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				const auto& config = state.config();
				auto pServiceGroup = state.pool().pushServiceGroup(Service_Name);
				auto pReaders = pServiceGroup->pushService(
						net::CreatePacketReaders,
						state.packetHandlers(),
						locator.keys().caPublicKey(),
						extensions::GetConnectionSettings(config),
						config.Node.MaxIncomingConnectionsPerIdentity);
				extensions::BootServer(
						*pServiceGroup,
						config.Node.Port,
						Service_Id,
						config,
						state.timeSupplier(),
						state.nodeSubscriber(),
						*pReaders);

				locator.registerService(Service_Name, pReaders);

				// add sinks
				state.hooks().addBannedNodeIdentitySink(extensions::CreateCloseConnectionSink(*pReaders));

				// add tasks
				state.tasks().push_back(CreateAgePeersTask(state, *pReaders));
			}
		};
	}

	DECLARE_SERVICE_REGISTRAR(NetworkPacketReaders)() {
		return std::make_unique<NetworkPacketReadersServiceRegistrar>();
	}
}}
