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

#include "TokenDefinitionBuilder.h"
#include "plugins/txes/token/src/model/TokenIdGenerator.h"

namespace bitxorcore { namespace builders {

	TokenDefinitionBuilder::TokenDefinitionBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_id()
			, m_duration()
			, m_nonce()
			, m_flags()
			, m_divisibility()
	{}

	void TokenDefinitionBuilder::setDuration(BlockDuration duration) {
		m_duration = duration;
	}

	void TokenDefinitionBuilder::setNonce(TokenNonce nonce) {
		m_nonce = nonce;
	}

	void TokenDefinitionBuilder::setFlags(model::TokenFlags flags) {
		m_flags = flags;
	}

	void TokenDefinitionBuilder::setDivisibility(uint8_t divisibility) {
		m_divisibility = divisibility;
	}

	size_t TokenDefinitionBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TokenDefinitionBuilder::Transaction> TokenDefinitionBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TokenDefinitionBuilder::EmbeddedTransaction> TokenDefinitionBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TokenDefinitionBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TokenDefinitionBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->Id = model::GenerateTokenId(model::GetSignerAddress(*pTransaction), m_nonce);
		pTransaction->Duration = m_duration;
		pTransaction->Nonce = m_nonce;
		pTransaction->Flags = m_flags;
		pTransaction->Divisibility = m_divisibility;

		return pTransaction;
	}
}}
