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

#include "bitxorcore/subscribers/BasicAggregateSubscriber.h"
#include "tests/bitxorcore/subscribers/test/AggregateSubscriberTestContext.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace subscribers {

#define TEST_CLASS BasicAggregateSubscriberTests

	namespace {
		// int is used as placeholder for a 'subscriber'
		class CapturingAggregateSubscriber : public BasicAggregateSubscriber<int> {
		public:
			using BasicAggregateSubscriber<int>::BasicAggregateSubscriber;

		public:
			std::vector<int*> captureAll() {
				std::vector<int*> subscribers;
				this->forEach([&subscribers](auto& subscriber) { subscribers.push_back(&subscriber); });
				return subscribers;
			}
		};

		using TestContext = test::AggregateSubscriberTestContext<int, CapturingAggregateSubscriber>;

		void RunDelegationTest(size_t numSubscribers) {
			// Arrange:
			TestContext context(numSubscribers);

			// Act:
			auto subscribers = context.aggregate().captureAll();

			// Assert:
			EXPECT_EQ(numSubscribers, subscribers.size());
			EXPECT_EQ(context.subscribers(), subscribers);
		}
	}

	TEST(TEST_CLASS, CanCreateAggregateAroundZeroSubscribers) {
		RunDelegationTest(0);
	}

	TEST(TEST_CLASS, CanCreateAggregateAroundSingleSubscriber) {
		RunDelegationTest(1);
	}

	TEST(TEST_CLASS, CanCreateAggregateAroundMultipleSubscribers) {
		RunDelegationTest(3);
	}
}}
