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

#include "NodeDiscoveryHandlers.h"
#include "nodediscovery/src/NodePingUtils.h"
#include "bitxorcore/handlers/BasicProducer.h"
#include "bitxorcore/handlers/HandlerFactory.h"
#include "bitxorcore/ionet/NetworkNode.h"
#include "bitxorcore/ionet/PacketPayloadFactory.h"
#include "bitxorcore/utils/HexFormatter.h"

namespace bitxorcore { namespace handlers {

	void RegisterNodeDiscoveryPushPingHandler(
			ionet::ServerPacketHandlers& handlers,
			const model::UniqueNetworkFingerprint& networkFingerprint,
			const NodeConsumer& nodeConsumer) {
		handlers.registerHandler(ionet::PacketType::Node_Discovery_Push_Ping, [networkFingerprint, nodeConsumer](
				const auto& packet,
				const auto& context) {
			ionet::Node node;
			if (!nodediscovery::TryParseNodePacket(packet, node))
				return;

			if (!nodediscovery::IsNodeCompatible(node, networkFingerprint, context.key())) {
				BITXORCORE_LOG(warning)
						<< "ignoring ping packet for incompatible node (identity = "
						<< node.identity() << ", network = " << node.metadata().NetworkFingerprint << ")";
				return;
			}

			auto identity = model::NodeIdentity{ node.identity().PublicKey, context.host() };
			auto endpoint = node.endpoint();

			if (endpoint.Host.empty()) {
				endpoint.Host = identity.Host;
				BITXORCORE_LOG(debug) << "auto detected host '" << endpoint.Host << "' for " << identity;
			}

			node = ionet::Node(identity, endpoint, node.metadata());
			BITXORCORE_LOG(debug) << "processing ping from " << node;
			nodeConsumer(node);
		});
	}

	void RegisterNodeDiscoveryPullPingHandler(
			ionet::ServerPacketHandlers& handlers,
			const std::shared_ptr<const ionet::NetworkNode>& pLocalNode) {
		handlers.registerHandler(ionet::PacketType::Node_Discovery_Pull_Ping, [pLocalNode](const auto& packet, auto& context) {
			if (!IsPacketValid(packet, ionet::PacketType::Node_Discovery_Pull_Ping))
				return;

			context.response(ionet::PacketPayloadFactory::FromEntity(ionet::PacketType::Node_Discovery_Pull_Ping, pLocalNode));
		});
	}

	void RegisterNodeDiscoveryPushPeersHandler(ionet::ServerPacketHandlers& handlers, const NodesConsumer& nodesConsumer) {
		handlers.registerHandler(ionet::PacketType::Node_Discovery_Push_Peers, [nodesConsumer](const auto& packet, const auto&) {
			ionet::NodeSet nodes;
			if (!nodediscovery::TryParseNodesPacket(packet, nodes) || nodes.empty())
				return;

			// nodes are not checked for compatibility here because they are being forwarded from a potentially malicious node
			// (incompatible nodes will be filtered out later by ping push handler)
			nodesConsumer(nodes);
		});
	}

	namespace {
		struct NodeDiscoveryPullPeersTraits {
			static constexpr auto Packet_Type = ionet::PacketType::Node_Discovery_Pull_Peers;

			class Producer : BasicProducer<ionet::NodeSet> {
			public:
				explicit Producer(const ionet::NodeSet& nodes) : BasicProducer<ionet::NodeSet>(nodes)
				{}

				auto operator()() {
					return next([](const auto& node) {
						return utils::UniqueToShared(ionet::PackNode(node));
					});
				}
			};
		};
	}

	void RegisterNodeDiscoveryPullPeersHandler(ionet::ServerPacketHandlers& handlers, const NodesSupplier& nodesSupplier) {
		handlers::BatchHandlerFactory<NodeDiscoveryPullPeersTraits>::RegisterZero(handlers, [nodesSupplier]() {
			auto pNodes = std::make_unique<ionet::NodeSet>(nodesSupplier()); // used by producer by reference
			auto producer = NodeDiscoveryPullPeersTraits::Producer(*pNodes);
			return [pNodes = std::move(pNodes), producer]() mutable {
				return producer();
			};
		});
	}
}}
