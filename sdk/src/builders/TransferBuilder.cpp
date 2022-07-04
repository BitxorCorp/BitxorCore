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

#include "TransferBuilder.h"

namespace bitxorcore { namespace builders {

	TransferBuilder::TransferBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_recipientAddress()
			, m_tokens()
			, m_message()
	{}

	void TransferBuilder::setRecipientAddress(const UnresolvedAddress& recipientAddress) {
		m_recipientAddress = recipientAddress;
	}

	void TransferBuilder::addToken(const model::UnresolvedToken& token) {
		InsertSorted(m_tokens, token, [](const auto& lhs, const auto& rhs) {
			return lhs.TokenId < rhs.TokenId;
		});
	}

	void TransferBuilder::setMessage(const RawBuffer& message) {
		if (0 == message.Size)
			BITXORCORE_THROW_INVALID_ARGUMENT("argument `message` cannot be empty");

		if (!m_message.empty())
			BITXORCORE_THROW_RUNTIME_ERROR("`message` field already set");

		m_message.resize(message.Size);
		m_message.assign(message.pData, message.pData + message.Size);
	}

	size_t TransferBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TransferBuilder::Transaction> TransferBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TransferBuilder::EmbeddedTransaction> TransferBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TransferBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		size += m_tokens.size() * sizeof(model::UnresolvedToken);
		size += m_message.size();
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TransferBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->RecipientAddress = m_recipientAddress;
		pTransaction->MessageSize = utils::checked_cast<size_t, uint16_t>(m_message.size());
		pTransaction->TokensCount = utils::checked_cast<size_t, uint8_t>(m_tokens.size());
		pTransaction->TransferTransactionBody_Reserved1 = 0;
		pTransaction->TransferTransactionBody_Reserved2 = 0;

		// 3. set transaction attachments
		std::copy(m_tokens.cbegin(), m_tokens.cend(), pTransaction->TokensPtr());
		std::copy(m_message.cbegin(), m_message.cend(), pTransaction->MessagePtr());

		return pTransaction;
	}
}}
