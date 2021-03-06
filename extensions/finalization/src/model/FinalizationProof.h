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
#include "FinalizationMessageGroup.h"
#include "bitxorcore/model/FinalizationRound.h"
#include "bitxorcore/model/SizePrefixedEntityContainer.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a finalization proof header.
	struct FinalizationProofHeader : public SizePrefixedEntity {
	public:
		/// Proof format version.
		static constexpr uint8_t Current_Version = 1;

	public:
		/// Proof version.
		uint32_t Version;

		/// Finalization round.
		model::FinalizationRound Round;

		/// Finalization height.
		bitxorcore::Height Height;

		/// Finalization hash.
		Hash256 Hash;
	};

	DEFINE_SIZE_PREFIXED_ENTITY_CONTAINER(MessageGroup, MessageGroups)

	/// Binary layout for a finalization proof.
	struct FinalizationProof : public MessageGroupContainer<FinalizationProofHeader, FinalizationMessageGroup> {};

#pragma pack(pop)

	/// Gets the number of bytes containing message group data according to \a header.
	size_t GetMessageGroupPayloadSize(const FinalizationProofHeader& header);

	/// Checks the real size of \a proof against its reported size and returns \c true if the sizes match.
	bool IsSizeValid(const FinalizationProof& proof);
}}
