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
#include "TransferEntityType.h"
#include "bitxorcore/model/Token.h"
#include "bitxorcore/model/Transaction.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a transfer transaction body.
	template<typename THeader>
	struct TransferTransactionBody : public THeader {
	private:
		using TransactionType = TransferTransactionBody<THeader>;

	public:
		DEFINE_TRANSACTION_CONSTANTS(Entity_Type_Transfer, 1)

	public:
		/// Recipient address.
		UnresolvedAddress RecipientAddress;

		/// Message size in bytes.
		uint16_t MessageSize;

		/// Number of tokens.
		uint8_t TokensCount;

		/// Reserved padding to align Tokens on 8-byte boundary.
		uint32_t TransferTransactionBody_Reserved1;
		uint8_t TransferTransactionBody_Reserved2;

		// followed by tokens data if TokensCount != 0
		DEFINE_TRANSACTION_VARIABLE_DATA_ACCESSORS(Tokens, UnresolvedToken)

		// followed by message data if MessageSize != 0
		DEFINE_TRANSACTION_VARIABLE_DATA_ACCESSORS(Message, uint8_t)

	private:
		template<typename T>
		static auto* TokensPtrT(T& transaction) {
			return transaction.TokensCount ? THeader::PayloadStart(transaction) : nullptr;
		}

		template<typename T>
		static auto* MessagePtrT(T& transaction) {
			auto* pPayloadStart = THeader::PayloadStart(transaction);
			return transaction.MessageSize && pPayloadStart
					? pPayloadStart + transaction.TokensCount * sizeof(UnresolvedToken)
					: nullptr;
		}

	public:
		/// Calculates the real size of transfer \a transaction.
		static constexpr uint64_t CalculateRealSize(const TransactionType& transaction) noexcept {
			return sizeof(TransactionType) + transaction.MessageSize + transaction.TokensCount * sizeof(UnresolvedToken);
		}
	};

	DEFINE_EMBEDDABLE_TRANSACTION(Transfer)

#pragma pack(pop)
}}
