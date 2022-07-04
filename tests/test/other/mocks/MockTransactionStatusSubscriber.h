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
#include "bitxorcore/model/Transaction.h"
#include "bitxorcore/model/TransactionStatus.h"
#include "bitxorcore/subscribers/TransactionStatusSubscriber.h"
#include "tests/test/core/EntityTestUtils.h"
#include "tests/test/nodeps/ParamsCapture.h"
#include <atomic>

namespace bitxorcore { namespace mocks {

	/// Transaction status subscriber status params.
	struct TransactionStatusSubscriberStatusParams {
	public:
		/// Creates params around \a transaction, \a hash and \a status.
		TransactionStatusSubscriberStatusParams(const model::Transaction& transaction, const Hash256& hash, uint32_t status)
				: Transaction(transaction)
				, pTransactionCopy(test::CopyEntity(transaction))
				, Hash(hash)
				, HashCopy(hash)
				, Status(status)
		{}

	public:
		/// Reference to the transaction.
		const model::Transaction& Transaction;

		/// Copy of the transaction.
		std::unique_ptr<model::Transaction> pTransactionCopy;

		/// Reference to the transaction hash.
		const Hash256& Hash;

		/// Copy of the transaction hash.
		Hash256 HashCopy;

		/// Transaction status.
		uint32_t Status;
	};

	/// Mock transaction status subscriber implementation.
	class MockTransactionStatusSubscriber
			: public subscribers::TransactionStatusSubscriber
			, public test::ParamsCapture<TransactionStatusSubscriberStatusParams> {
	public:
		/// Creates a subscriber.
		MockTransactionStatusSubscriber()
				: m_numNotifies(0)
				, m_numFlushes(0)
		{}

	public:
		/// Gets the number of notifyStatus calls.
		size_t numNotifies() const {
			return m_numNotifies;
		}

		/// Gets the number of flush calls.
		size_t numFlushes() const {
			return m_numFlushes;
		}

	public:
		void notifyStatus(const model::Transaction& transaction, const Hash256& hash, uint32_t status) override {
			push(transaction, hash, status);
			++m_numNotifies;
		}

		void flush() override {
			++m_numFlushes;
		}

	private:
		std::atomic<size_t> m_numNotifies;
		std::atomic<size_t> m_numFlushes;
	};
}}
