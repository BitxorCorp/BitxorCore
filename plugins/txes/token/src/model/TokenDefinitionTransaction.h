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
#include "TokenConstants.h"
#include "TokenEntityType.h"
#include "TokenProperties.h"
#include "bitxorcore/model/Transaction.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a token definition transaction body.
	template<typename THeader>
	struct TokenDefinitionTransactionBody : public THeader {
	private:
		using TransactionType = TokenDefinitionTransactionBody<THeader>;

	public:
		DEFINE_TRANSACTION_CONSTANTS(Entity_Type_Token_Definition, 1)

	public:
		/// Token identifier.
		/// \note This must match the generated id.
		TokenId Id;

		/// Token duration
		BlockDuration Duration;

		/// Token nonce.
		TokenNonce Nonce;

		/// Token flags.
		TokenFlags Flags;

		/// Token divisibility.
		uint8_t Divisibility;

	public:
		/// Calculates the real size of token definition \a transaction.
		static constexpr uint64_t CalculateRealSize(const TransactionType&) noexcept {
			return sizeof(TransactionType);
		}
	};

	DEFINE_EMBEDDABLE_TRANSACTION(TokenDefinition)

#pragma pack(pop)
}}
