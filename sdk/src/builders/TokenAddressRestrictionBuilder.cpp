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

#include "TokenAddressRestrictionBuilder.h"

namespace bitxorcore { namespace builders {

	TokenAddressRestrictionBuilder::TokenAddressRestrictionBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_tokenId()
			, m_restrictionKey()
			, m_previousRestrictionValue()
			, m_newRestrictionValue()
			, m_targetAddress()
	{}

	void TokenAddressRestrictionBuilder::setTokenId(UnresolvedTokenId tokenId) {
		m_tokenId = tokenId;
	}

	void TokenAddressRestrictionBuilder::setRestrictionKey(uint64_t restrictionKey) {
		m_restrictionKey = restrictionKey;
	}

	void TokenAddressRestrictionBuilder::setPreviousRestrictionValue(uint64_t previousRestrictionValue) {
		m_previousRestrictionValue = previousRestrictionValue;
	}

	void TokenAddressRestrictionBuilder::setNewRestrictionValue(uint64_t newRestrictionValue) {
		m_newRestrictionValue = newRestrictionValue;
	}

	void TokenAddressRestrictionBuilder::setTargetAddress(const UnresolvedAddress& targetAddress) {
		m_targetAddress = targetAddress;
	}

	size_t TokenAddressRestrictionBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<TokenAddressRestrictionBuilder::Transaction> TokenAddressRestrictionBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<TokenAddressRestrictionBuilder::EmbeddedTransaction> TokenAddressRestrictionBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t TokenAddressRestrictionBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> TokenAddressRestrictionBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->TokenId = m_tokenId;
		pTransaction->RestrictionKey = m_restrictionKey;
		pTransaction->PreviousRestrictionValue = m_previousRestrictionValue;
		pTransaction->NewRestrictionValue = m_newRestrictionValue;
		pTransaction->TargetAddress = m_targetAddress;

		return pTransaction;
	}
}}
