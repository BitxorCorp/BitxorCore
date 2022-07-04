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

#include "TokenGlobalRestrictionBuilder.h"

namespace bitxorcore { namespace builders {

	TokenGlobalRestrictionBuilder::TokenGlobalRestrictionBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_tokenId()
			, m_referenceTokenId()
			, m_restrictionKey()
			, m_previousRestrictionValue()
			, m_newRestrictionValue()
			, m_previousRestrictionType()
			, m_newRestrictionType()
	{}

	void TokenGlobalRestrictionBuilder::setTokenId(UnresolvedTokenId tokenId) {
		m_tokenId = tokenId;
	}

	void TokenGlobalRestrictionBuilder::setReferenceTokenId(UnresolvedTokenId referenceTokenId) {
		m_referenceTokenId = referenceTokenId;
	}

	void TokenGlobalRestrictionBuilder::setRestrictionKey(uint64_t restrictionKey) {
		m_restrictionKey = restrictionKey;
	}

	void TokenGlobalRestrictionBuilder::setPreviousRestrictionValue(uint64_t previousRestrictionValue) {
		m_previousRestrictionValue = previousRestrictionValue;
	}

	void TokenGlobalRestrictionBuilder::setNewRestrictionValue(uint64_t newRestrictionValue) {
		m_newRestrictionValue = newRestrictionValue;
	}

	void TokenGlobalRestrictionBuilder::setPreviousRestrictionType(model::TokenRestrictionType previousRestrictionType) {
		m_previousRestrictionType = previousRestrictionType;
	}

	void TokenGlobalRestrictionBuilder::setNewRestrictionType(model::TokenRestrictionType newRestrictionType) {
		m_newRestrictionType = newRestrictionType;
	}

	size_t TokenGlobalRestrictionBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TokenGlobalRestrictionBuilder::Transaction> TokenGlobalRestrictionBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TokenGlobalRestrictionBuilder::EmbeddedTransaction> TokenGlobalRestrictionBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TokenGlobalRestrictionBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TokenGlobalRestrictionBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->TokenId = m_tokenId;
		pTransaction->ReferenceTokenId = m_referenceTokenId;
		pTransaction->RestrictionKey = m_restrictionKey;
		pTransaction->PreviousRestrictionValue = m_previousRestrictionValue;
		pTransaction->NewRestrictionValue = m_newRestrictionValue;
		pTransaction->PreviousRestrictionType = m_previousRestrictionType;
		pTransaction->NewRestrictionType = m_newRestrictionType;

		return pTransaction;
	}
}}
