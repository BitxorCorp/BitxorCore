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

#include "NetworkPacketWritersService.h"
#include "bitxorcore/api/RemoteChainApi.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/extensions/NetworkUtils.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/ionet/BroadcastUtils.h"
#include "bitxorcore/thread/FutureUtils.h"
#include "bitxorcore/thread/MultiServicePool.h"

namespace bitxorcore { namespace sync {

	namespace {
		constexpr auto Service_Name = "writers";
		using BlockSink = extensions::NewBlockSink;
		using TransactionsSink = extensions::SharedNewTransactionsSink;

		extensions::RemoteChainHeightsRetriever CreateRemoteChainHeightsRetriever(net::PacketIoPicker& packetIoPicker) {
			return [&packetIoPicker](auto numPeers) {
				std::vector<thread::future<Height>> heightFutures;
				auto timeout = utils::TimeSpan::FromSeconds(5);

				auto packetIoPairs = net::PickMultiple(packetIoPicker, numPeers, timeout);
				if (packetIoPairs.empty()) {
					BITXORCORE_LOG(warning) << "could not find any peer for detecting chain heights";
					return thread::make_ready_future(std::vector<Height>());
				}

				for (const auto& packetIoPair : packetIoPairs) {
					auto pPacketIo = packetIoPair.io();
					auto pChainApi = api::CreateRemoteChainApiWithoutRegistry(*pPacketIo);
					heightFutures.push_back(pChainApi->chainStatistics().then([pPacketIo](auto&& chainStatisticsFuture) {
						return chainStatisticsFuture.get().Height;
					}));
				}

				return thread::when_all(std::move(heightFutures)).then([](auto&& completedFutures) {
					return thread::get_all_ignore_exceptional(completedFutures.get());
				});
			};
		}

		class NetworkPacketWritersServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "NetworkPacketWriters", extensions::ServiceRegistrarPhase::Initial };
			}

			void registerServiceCounters(extensions::ServiceLocator& locator) override {
				locator.registerServiceCounter<net::PacketWriters>(Service_Name, "WRITERS", [](const auto& writers) {
					return writers.numActiveWriters();
				});
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				auto connectionSettings = extensions::GetConnectionSettings(state.config());
				auto pServiceGroup = state.pool().pushServiceGroup(Service_Name);
				auto pWriters = pServiceGroup->pushService(net::CreatePacketWriters, locator.keys().caPublicKey(), connectionSettings);

				locator.registerService(Service_Name, pWriters);
				state.packetIoPickers().insert(*pWriters, ionet::NodeRoles::Peer);

				// add sinks
				state.hooks().addNewBlockSink(extensions::CreatePushEntitySink<BlockSink>(locator, Service_Name));
				state.hooks().addNewTransactionsSink(extensions::CreatePushEntitySink<TransactionsSink>(locator, Service_Name));
				state.hooks().addPacketPayloadSink([&writers = *pWriters](const auto& payload) { writers.broadcast(payload); });
				state.hooks().addBannedNodeIdentitySink(extensions::CreateCloseConnectionSink(*pWriters));

				// add retrievers
				state.hooks().setRemoteChainHeightsRetriever(CreateRemoteChainHeightsRetriever(*pWriters));
			}
		};
	}

	std::shared_ptr<net::PacketWriters> GetPacketWriters(const extensions::ServiceLocator& locator) {
		return locator.service<net::PacketWriters>(Service_Name);
	}

	DECLARE_SERVICE_REGISTRAR(NetworkPacketWriters)() {
		return std::make_unique<NetworkPacketWritersServiceRegistrar>();
	}
}}
