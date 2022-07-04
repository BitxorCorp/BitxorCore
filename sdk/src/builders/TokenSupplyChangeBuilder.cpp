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

#include "TokenSupplyChangeBuilder.h"

namespace bitxorcore { namespace builders {

	TokenSupplyChangeBuilder::TokenSupplyChangeBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_tokenId()
			, m_delta()
			, m_action()
	{}

	void TokenSupplyChangeBuilder::setTokenId(UnresolvedTokenId tokenId) {
		m_tokenId = tokenId;
	}

	void TokenSupplyChangeBuilder::setDelta(Amount delta) {
		m_delta = delta;
	}

	void TokenSupplyChangeBuilder::setAction(model::TokenSupplyChangeAction action) {
		m_action = action;
	}

	size_t TokenSupplyChangeBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TokenSupplyChangeBuilder::Transaction> TokenSupplyChangeBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TokenSupplyChangeBuilder::EmbeddedTransaction> TokenSupplyChangeBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TokenSupplyChangeBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TokenSupplyChangeBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->TokenId = m_tokenId;
		pTransaction->Delta = m_delta;
		pTransaction->Action = m_action;

		return pTransaction;
	}
}}
