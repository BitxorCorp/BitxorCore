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
#include "StepIdentifier.h"
#include "bitxorcore/crypto_voting/BmTreeSignature.h"
#include "bitxorcore/model/SizePrefixedEntity.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Finalization message group.
	struct FinalizationMessageGroup : public SizePrefixedEntity {
	private:
		template<typename T>
		static auto* HashesPtrT(T& messageGroup) {
			return messageGroup.HashesCount ? PayloadStart(messageGroup) : nullptr;
		}

		template<typename T>
		static auto* SignaturesPtrT(T& messageGroup) {
			auto* pPayloadStart = PayloadStart(messageGroup);
			return messageGroup.SignaturesCount && pPayloadStart
					? pPayloadStart + messageGroup.HashesCount * Hash256::Size
					: nullptr;
		}

	public:
		/// Number of hashes.
		uint32_t HashesCount;

		/// Number of signatures.
		uint32_t SignaturesCount;

		/// Message stage.
		FinalizationStage Stage;

		/// Block height corresponding to the the first hash.
		bitxorcore::Height Height;

		// followed by hashes data if HashesCount != 0
		DEFINE_SIZE_PREFIXED_ENTITY_VARIABLE_DATA_ACCESSORS(Hashes, Hash256)

		// followed by signature data if SignaturesCount != 0
		DEFINE_SIZE_PREFIXED_ENTITY_VARIABLE_DATA_ACCESSORS(Signatures, crypto::BmTreeSignature)

	public:
		/// Calculates the real size of finalization message group (\a messageGroup).
		static constexpr uint64_t CalculateRealSize(const FinalizationMessageGroup& messageGroup) noexcept {
			return sizeof(FinalizationMessageGroup)
					+ messageGroup.HashesCount * Hash256::Size
					+ messageGroup.SignaturesCount * sizeof(crypto::BmTreeSignature);
		}
	};

#pragma pack(pop)
}}
