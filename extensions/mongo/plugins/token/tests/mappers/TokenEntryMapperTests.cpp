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

#include "src/mappers/TokenEntryMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/token/src/state/TokenEntry.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "plugins/txes/token/tests/test/TokenTestUtils.h"
#include "tests/test/TokenMapperTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS TokenEntryMapperTests

	// region ToDbModel

	namespace {
		state::TokenEntry CreateTokenEntry() {
			auto owner = test::CreateRandomOwner();
			return test::CreateTokenEntry(TokenId(345), Height(123), owner, Amount(456), BlockDuration(12345));
		}
	}

	TEST(TEST_CLASS, CanMapTokenEntry_ModelToDbModel) {
		// Arrange:
		auto entry = CreateTokenEntry();

		// Act:
		auto document = ToDbModel(entry);
		auto documentView = document.view();

		// Assert:
		EXPECT_EQ(1u, test::GetFieldCount(documentView));

		auto tokenView = documentView["token"].get_document().view();
		test::AssertEqualTokenData(entry, tokenView);
	}

	// endregion
}}}
