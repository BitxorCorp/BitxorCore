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

#include "HashLockBuilder.h"

namespace bitxorcore { namespace builders {

	HashLockBuilder::HashLockBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer)
			: TransactionBuilder(networkIdentifier, signer)
			, m_token()
			, m_duration()
			, m_hash()
	{}

	void HashLockBuilder::setToken(const model::UnresolvedToken& token) {
		m_token = token;
	}

	void HashLockBuilder::setDuration(BlockDuration duration) {
		m_duration = duration;
	}

	void HashLockBuilder::setHash(const Hash256& hash) {
		m_hash = hash;
	}

	size_t HashLockBuilder::size() const {
		return sizeImpl<Transaction>();
	}

	std::unique_ptr<HashLockBuilder::Transaction> HashLockBuilder::build() const {
		return buildImpl<Transaction>();
	}

	std::unique_ptr<HashLockBuilder::EmbeddedTransaction> HashLockBuilder::buildEmbedded() const {
		return buildImpl<EmbeddedTransaction>();
	}

	template<typename TransactionType>
	size_t HashLockBuilder::sizeImpl() const {
		// calculate transaction size
		auto size = sizeof(TransactionType);
		return size;
	}

	template<typename TransactionType>
	std::unique_ptr<TransactionType> HashLockBuilder::buildImpl() const {
		// 1. allocate, zero (header), set model::Transaction fields
		auto pTransaction = createTransaction<TransactionType>(sizeImpl<TransactionType>());

		// 2. set fixed transaction fields
		pTransaction->Token = m_token;
		pTransaction->Duration = m_duration;
		pTransaction->Hash = m_hash;

		return pTransaction;
	}
}}
