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

#include "MerkleHandlers.h"
#include "HeightRequestProcessor.h"
#include "bitxorcore/api/ChainPackets.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/ionet/PacketPayloadFactory.h"

namespace bitxorcore { namespace handlers {

	namespace {
		model::HashRange ToHashRange(const std::vector<Hash256>& hashes) {
			return model::HashRange::CopyFixed(reinterpret_cast<const uint8_t*>(hashes.data()), hashes.size());
		}

		auto CreateSubCacheMerkleRootsHandler(const io::BlockStorageCache& storage) {
			return [&storage](const auto& packet, auto& context) {
				using RequestType = api::HeightPacket<ionet::PacketType::Sub_Cache_Merkle_Roots>;
				auto storageView = storage.view();
				auto info = HeightRequestProcessor<RequestType>::Process(storageView, packet, context, false);
				if (!info.pRequest)
					return;

				auto pBlockElement = storageView.loadBlockElement(info.pRequest->Height);

				auto hashRange = ToHashRange(pBlockElement->SubCacheMerkleRoots);
				auto payload = ionet::PacketPayloadFactory::FromFixedSizeRange(RequestType::Packet_Type, std::move(hashRange));
				context.response(std::move(payload));
			};
		}
	}

	void RegisterSubCacheMerkleRootsHandler(ionet::ServerPacketHandlers& handlers, const io::BlockStorageCache& storage) {
		handlers.registerHandler(ionet::PacketType::Sub_Cache_Merkle_Roots, CreateSubCacheMerkleRootsHandler(storage));
	}
}}
