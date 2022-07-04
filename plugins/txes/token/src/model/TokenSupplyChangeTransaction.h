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
#include "TokenEntityType.h"
#include "TokenTypes.h"
#include "bitxorcore/model/Transaction.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a token supply change transaction body.
	template<typename THeader>
	struct TokenSupplyChangeTransactionBody : public THeader {
	private:
		using TransactionType = TokenSupplyChangeTransactionBody<THeader>;

	public:
		DEFINE_TRANSACTION_CONSTANTS(Entity_Type_Token_Supply_Change, 1)

	public:
		/// Affected token identifier.
		UnresolvedTokenId TokenId;

		/// Change amount.
		Amount Delta;

		/// Supply change action.
		TokenSupplyChangeAction Action;

	public:
		/// Calculates the real size of a token supply change \a transaction.
		static constexpr uint64_t CalculateRealSize(const TransactionType&) noexcept {
			return sizeof(TransactionType);
		}
	};

	DEFINE_EMBEDDABLE_TRANSACTION(TokenSupplyChange)

#pragma pack(pop)
}}
