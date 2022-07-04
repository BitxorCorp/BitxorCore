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

#include "RemoteFinalizationApi.h"
#include "finalization/src/model/FinalizationMessage.h"
#include "bitxorcore/api/RemoteRequestDispatcher.h"
#include "bitxorcore/ionet/PacketEntityUtils.h"
#include "bitxorcore/ionet/PacketPayloadBuilder.h"

namespace bitxorcore { namespace api {

	namespace {
		// region traits

		struct MessagesTraits {
		public:
			using ResultType = model::FinalizationMessageRange;
			static constexpr auto Packet_Type = ionet::PacketType::Pull_Finalization_Messages;
			static constexpr auto Friendly_Name = "pull finalization messages";

			static auto CreateRequestPacketPayload(
					const model::FinalizationRoundRange& roundRange,
					model::ShortHashRange&& knownShortHashes) {
				ionet::PacketPayloadBuilder builder(Packet_Type);
				builder.appendValue(roundRange);
				builder.appendRange(std::move(knownShortHashes));
				return builder.build();
			}

		public:
			bool tryParseResult(const ionet::Packet& packet, ResultType& result) const {
				result = ionet::ExtractEntitiesFromPacket<model::FinalizationMessage>(
						packet,
						model::IsSizeValidT<model::FinalizationMessage>);
				return !result.empty() || sizeof(ionet::PacketHeader) == packet.Size;
			}
		};

		// endregion

		class DefaultRemoteFinalizationApi : public RemoteFinalizationApi {
		private:
			template<typename TTraits>
			using FutureType = thread::future<typename TTraits::ResultType>;

		public:
			DefaultRemoteFinalizationApi(ionet::PacketIo& io, const model::NodeIdentity& remoteIdentity)
					: RemoteFinalizationApi(remoteIdentity)
					, m_impl(io)
			{}

		public:
			FutureType<MessagesTraits> messages(
					const model::FinalizationRoundRange& roundRange,
					model::ShortHashRange&& knownShortHashes) const override {
				return m_impl.dispatch(MessagesTraits(), roundRange, std::move(knownShortHashes));
			}

		private:
			mutable RemoteRequestDispatcher m_impl;
		};
	}

	std::unique_ptr<RemoteFinalizationApi> CreateRemoteFinalizationApi(ionet::PacketIo& io, const model::NodeIdentity& remoteIdentity) {
		return std::make_unique<DefaultRemoteFinalizationApi>(io, remoteIdentity);
	}
}}
