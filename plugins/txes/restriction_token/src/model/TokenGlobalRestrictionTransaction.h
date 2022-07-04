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
#include "TokenRestrictionEntityType.h"
#include "TokenRestrictionTypes.h"
#include "bitxorcore/model/Transaction.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a token global restriction transaction body.
	template<typename THeader>
	struct TokenGlobalRestrictionTransactionBody : public THeader {
	private:
		using TransactionType = TokenGlobalRestrictionTransactionBody<THeader>;

	public:
		DEFINE_TRANSACTION_CONSTANTS(Entity_Type_Token_Global_Restriction, 1)

	public:
		/// Identifier of the token being restricted.
		UnresolvedTokenId TokenId;

		/// Identifier of the token providing the restriction key.
		UnresolvedTokenId ReferenceTokenId;

		/// Restriction key relative to the reference token identifier.
		uint64_t RestrictionKey;

		/// Previous restriction value.
		uint64_t PreviousRestrictionValue;

		/// New restriction value.
		uint64_t NewRestrictionValue;

		/// Previous restriction type.
		TokenRestrictionType PreviousRestrictionType;

		/// New restriction type.
		TokenRestrictionType NewRestrictionType;

	public:
		/// Calculates the real size of a token global restriction \a transaction.
		static constexpr uint64_t CalculateRealSize(const TransactionType&) noexcept {
			return sizeof(TransactionType);
		}
	};

	DEFINE_EMBEDDABLE_TRANSACTION(TokenGlobalRestriction)

#pragma pack(pop)
}}
