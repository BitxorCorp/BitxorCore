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

#include "bitxorcore/subscribers/AggregateTransactionStatusSubscriber.h"
#include "bitxorcore/model/TransactionStatus.h"
#include "tests/bitxorcore/subscribers/test/AggregateSubscriberTestContext.h"
#include "tests/test/core/TransactionTestUtils.h"
#include "tests/test/other/mocks/MockTransactionStatusSubscriber.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace subscribers {

#define TEST_CLASS AggregateTransactionStatusSubscriberTests

	namespace {
		template<typename TTransactionStatusSubscriber>
		using TestContext = test::AggregateSubscriberTestContext<
				TTransactionStatusSubscriber,
				AggregateTransactionStatusSubscriber<TTransactionStatusSubscriber>>;
	}

	TEST(TEST_CLASS, NotifyStatusForwardsToAllSubscribers) {
		// Arrange:
		TestContext<mocks::MockTransactionStatusSubscriber> context;
		auto pTransaction = test::GenerateRandomTransaction();
		auto hash = test::GenerateRandomByteArray<Hash256>();

		// Sanity:
		EXPECT_EQ(3u, context.subscribers().size());

		// Act:
		context.aggregate().notifyStatus(*pTransaction, hash, 123);

		// Assert:
		auto i = 0u;
		for (const auto* pSubscriber : context.subscribers()) {
			auto message = "subscriber at " + std::to_string(i++);
			const auto& capturedParams = pSubscriber->params();
			ASSERT_EQ(1u, capturedParams.size()) << message;
			EXPECT_EQ(pTransaction.get(), &capturedParams[0].Transaction) << message;
			EXPECT_EQ(hash, capturedParams[0].Hash) << message;
			EXPECT_EQ(123u, capturedParams[0].Status) << message;
		}
	}

	TEST(TEST_CLASS, FlushForwardsToAllSubscribers) {
		// Arrange:
		TestContext<mocks::MockTransactionStatusSubscriber> context;

		// Sanity:
		EXPECT_EQ(3u, context.subscribers().size());

		// Act:
		context.aggregate().flush();

		// Assert:
		auto i = 0u;
		for (const auto* pSubscriber : context.subscribers()) {
			auto message = "subscriber at " + std::to_string(i++);
			ASSERT_EQ(1u, pSubscriber->numFlushes()) << message;
		}
	}
}}
