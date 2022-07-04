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

#include "bitxorcore/subscribers/TransactionStatusReader.h"
#include "tests/test/core/TransactionStatusTestUtils.h"
#include "tests/test/core/mocks/MockMemoryStream.h"
#include "tests/test/other/mocks/MockTransactionStatusSubscriber.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace subscribers {

#define TEST_CLASS TransactionStatusReaderTests

	namespace {
		void AssertEqual(
				const test::TransactionStatusNotification& expected,
				const mocks::TransactionStatusSubscriberStatusParams& actual,
				const std::string& message) {
			EXPECT_EQ(*expected.pTransaction, *actual.pTransactionCopy) << message;
			EXPECT_EQ(expected.Hash, actual.HashCopy) << message;
			EXPECT_EQ(expected.Status, actual.Status) << message;
		}
	}

	TEST(TEST_CLASS, CanReadSingle) {
		// Arrange:
		auto notification = test::GenerateRandomTransactionStatusNotification(141);

		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream stream(buffer);
		test::WriteTransactionStatusNotification(stream, notification);
		stream.seek(0);

		mocks::MockTransactionStatusSubscriber subscriber;

		// Act:
		ReadNextTransactionStatus(stream, subscriber);

		// Assert:
		ASSERT_EQ(1u, subscriber.numNotifies());
		AssertEqual(notification, subscriber.params()[0], "at 0");

		EXPECT_EQ(0u, subscriber.numFlushes());
	}
}}
