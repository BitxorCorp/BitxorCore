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

#include "ZeroMqTransactionStatusSubscriber.h"
#include "ZeroMqEntityPublisher.h"
#include "bitxorcore/model/Transaction.h"
#include "bitxorcore/model/TransactionStatus.h"

namespace bitxorcore { namespace zeromq {

	namespace {
		class ZeroMqTransactionStatusSubscriber : public subscribers::TransactionStatusSubscriber {
		public:
			explicit ZeroMqTransactionStatusSubscriber(ZeroMqEntityPublisher& publisher) : m_publisher(publisher)
			{}

		public:
			void notifyStatus(const model::Transaction& transaction, const Hash256& hash, uint32_t status) override {
				m_publisher.publishTransactionStatus(transaction, hash, status);
			}

			void flush() override {
				// empty since the publisher will flush all pending statuses periodically
			}

		private:
			ZeroMqEntityPublisher& m_publisher;
		};
	}

	std::unique_ptr<subscribers::TransactionStatusSubscriber> CreateZeroMqTransactionStatusSubscriber(ZeroMqEntityPublisher& publisher) {
		return std::make_unique<ZeroMqTransactionStatusSubscriber>(publisher);
	}
}}
