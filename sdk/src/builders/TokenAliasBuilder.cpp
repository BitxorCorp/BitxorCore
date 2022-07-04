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

#include "TokenAliasBuilder.h"

namespace bitxorcore { namespace builders {

	TokenAliasBuilder::TokenAliasBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_namespaceId()
			, m_tokenId()
			, m_aliasAction()
	{}

	void TokenAliasBuilder::setNamespaceId(NamespaceId namespaceId) {
		m_namespaceId = namespaceId;
	}

	void TokenAliasBuilder::setTokenId(TokenId tokenId) {
		m_tokenId = tokenId;
	}

	void TokenAliasBuilder::setAliasAction(model::AliasAction aliasAction) {
		m_aliasAction = aliasAction;
	}

	size_t TokenAliasBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TokenAliasBuilder::Transaction> TokenAliasBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TokenAliasBuilder::EmbeddedTransaction> TokenAliasBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TokenAliasBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TokenAliasBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->NamespaceId = m_namespaceId;
		pTransaction->TokenId = m_tokenId;
		pTransaction->AliasAction = m_aliasAction;

		return pTransaction;
	}
}}
