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

#pragma once
#include "bitxorcore/ionet/Packet.h"
#include "bitxorcore/model/FinalizationRound.h"

namespace bitxorcore { namespace api {

#pragma pack(push, 1)

	/// Finalization statistics response.
	struct FinalizationStatisticsResponse : public ionet::Packet {
		static constexpr ionet::PacketType Packet_Type = ionet::PacketType::Finalization_Statistics;

		/// Finalization round.
		model::FinalizationRound Round;

		/// Finalization height.
		bitxorcore::Height Height;

		/// Finalization hash.
		Hash256 Hash;
	};

	/// Request packet for a proof at a finalization epoch.
	struct ProofAtEpochRequest : public ionet::Packet {
		static constexpr ionet::PacketType Packet_Type = ionet::PacketType::Finalization_Proof_At_Epoch;

		/// Requested finalization proof epoch.
		FinalizationEpoch Epoch;
	};

	/// Request packet for a proof at a finalization height.
	struct ProofAtHeightRequest : public ionet::Packet {
		static constexpr ionet::PacketType Packet_Type = ionet::PacketType::Finalization_Proof_At_Height;

		/// Requested finalization proof height.
		bitxorcore::Height Height;
	};

#pragma pack(pop)
}}
