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

#include "TokenRestrictionTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	state::TokenRestrictionEntry GenerateTokenRestrictionEntry(const Hash256& hash) {
		// hack to set the token restriction unique key (required for cache tests)
		auto addressRestriction = state::TokenAddressRestriction(
				test::GenerateRandomValue<TokenId>(),
				test::GenerateRandomByteArray<Address>());
		auto entry = state::TokenRestrictionEntry(addressRestriction);
		const_cast<Hash256&>(entry.uniqueKey()) = hash;
		return entry;
	}

	namespace {
		void AssertEqual(const state::TokenAddressRestriction& expected, const state::TokenAddressRestriction& actual) {
			EXPECT_EQ(expected.tokenId(), actual.tokenId());
			EXPECT_EQ(expected.address(), actual.address());
			EXPECT_EQ(expected.keys(), actual.keys());

			for (auto key : expected.keys())
				EXPECT_EQ(expected.get(key), actual.get(key)) << "key " << key;
		}

		void AssertEqual(const state::TokenGlobalRestriction& expected, const state::TokenGlobalRestriction& actual) {
			EXPECT_EQ(expected.tokenId(), actual.tokenId());
			EXPECT_EQ(expected.keys(), actual.keys());

			for (auto key : expected.keys()) {
				state::TokenGlobalRestriction::RestrictionRule expectedRule;
				state::TokenGlobalRestriction::RestrictionRule actualRule;
				expected.tryGet(key, expectedRule);
				actual.tryGet(key, actualRule);

				EXPECT_EQ(expectedRule.ReferenceTokenId, actualRule.ReferenceTokenId) << "key " << key;
				EXPECT_EQ(expectedRule.RestrictionValue, actualRule.RestrictionValue) << "key " << key;
				EXPECT_EQ(expectedRule.RestrictionType, actualRule.RestrictionType) << "key " << key;
			}
		}
	}

	void AssertEqual(const state::TokenRestrictionEntry& expected, const state::TokenRestrictionEntry& actual) {
		ASSERT_EQ(expected.entryType(), actual.entryType());

		if (state::TokenRestrictionEntry::EntryType::Address == expected.entryType())
			AssertEqual(expected.asAddressRestriction(), actual.asAddressRestriction());
		else
			AssertEqual(expected.asGlobalRestriction(), actual.asGlobalRestriction());
	}
}}
