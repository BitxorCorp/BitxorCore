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
#include "EntityBody.h"
#include "SizePrefixedEntity.h"

namespace bitxorcore { namespace model { class TransactionRegistry; } }

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a verifiable entity header.
	struct VerifiableEntityHeader : public SizePrefixedEntity {
	public:
		/// Size of the header that can be skipped when signing/verifying.
		static constexpr size_t Header_Size = sizeof(SizePrefixedEntity) + 7 + bitxorcore::Signature::Size + Key::Size;

	public:
		/// Reserved padding to align Signature on 8-byte boundary.
		uint32_t VerifiableEntityHeader_Reserved1;

		/// Entity signature.
		bitxorcore::Signature Signature;
	};

	/// Binary layout for a verifiable (with signature) entity.
	struct VerifiableEntity : public EntityBody<VerifiableEntityHeader> {};

#pragma pack(pop)

	/// Insertion operator for outputting \a entity to \a out.
	std::ostream& operator<<(std::ostream& out, const VerifiableEntity& entity);

	/// Gets the address of the signer of \a entity.
	Address GetSignerAddress(const VerifiableEntity& entity);

	/// Checks the real size of \a entity against its reported size and returns \c true if the sizes match.
	/// \a registry contains all known transaction types.
	bool IsSizeValid(const VerifiableEntity& entity, const TransactionRegistry& registry);
}}
