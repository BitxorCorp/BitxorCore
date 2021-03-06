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

#include "mongo/src/mappers/TransactionStatusMapper.h"
#include "bitxorcore/model/TransactionStatus.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/TestHarness.h"
#include <mongocxx/client.hpp>

namespace bitxorcore { namespace mongo { namespace mappers {

#define TEST_CLASS TransactionStatusMapperTests

	TEST(TEST_CLASS, CanMapTransactionStatus) {
		// Arrange:
		auto hash = test::GenerateRandomByteArray<Hash256>();

		// Act:
		auto document = ToDbModel(model::TransactionStatus(hash, Timestamp(321), 123456));
		auto documentView = document.view();

		// Assert:
		EXPECT_EQ(1u, test::GetFieldCount(documentView));

		auto statusView = documentView["status"].get_document().view();
		EXPECT_EQ(3u, test::GetFieldCount(statusView));

		EXPECT_EQ(hash, test::GetHashValue(statusView, "hash"));
		EXPECT_EQ(123456u, test::GetUint32(statusView, "code"));
		EXPECT_EQ(321u, test::GetUint64(statusView, "deadline"));
	}
}}}
