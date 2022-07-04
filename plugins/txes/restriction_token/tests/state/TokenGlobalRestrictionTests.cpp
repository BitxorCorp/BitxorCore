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

#include "src/state/TokenGlobalRestriction.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenGlobalRestrictionTests

	// region ctor

	TEST(TEST_CLASS, CanCreateRestriction) {
		// Act:
		TokenGlobalRestriction restriction(TokenId(123));

		// Assert:
		EXPECT_EQ(TokenId(123), restriction.tokenId());

		EXPECT_EQ(0u, restriction.size());
		EXPECT_EQ(std::set<uint64_t>(), restriction.keys());
	}

	// endregion

	// region get

	TEST(TEST_CLASS, CannotGetValueForUnsetRestriction) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });

		// Act:
		TokenGlobalRestriction::RestrictionRule rule;
		auto result = restriction.tryGet(112, rule);

		// Assert:
		EXPECT_FALSE(result);
	}

	TEST(TEST_CLASS, CanGetValueForSetRestriction) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });

		// Act:
		TokenGlobalRestriction::RestrictionRule rule;
		auto result = restriction.tryGet(111, rule);

		// Assert:
		EXPECT_TRUE(result);
		EXPECT_EQ(TokenId(17), rule.ReferenceTokenId);
		EXPECT_EQ(444u, rule.RestrictionValue);
		EXPECT_EQ(model::TokenRestrictionType::LT, rule.RestrictionType);
	}

	// endregion

	// region set

	namespace {
		void AssertCanGetValue(const TokenGlobalRestriction& restriction, uint64_t key, uint64_t expectedValue) {
			TokenGlobalRestriction::RestrictionRule rule;
			auto result = restriction.tryGet(key, rule);

			EXPECT_TRUE(result) << "for key " << key;
			EXPECT_EQ(expectedValue, rule.RestrictionValue) << "for key " << key;
		}

		void AssertCannotGetValue(const TokenGlobalRestriction& restriction, uint64_t key) {
			TokenGlobalRestriction::RestrictionRule rule;
			auto result = restriction.tryGet(key, rule);

			EXPECT_FALSE(result) << "for key " << key;
		}
	}

	TEST(TEST_CLASS, CanSetSingleValue) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));

		// Act:
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });

		// Assert:
		EXPECT_EQ(1u, restriction.size());
		AssertCanGetValue(restriction, 111, 444);
		EXPECT_EQ(std::set<uint64_t>({ 111 }), restriction.keys());
	}

	TEST(TEST_CLASS, CannotSetSentinelValue) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));

		// Act:
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::NONE });

		// Assert:
		EXPECT_EQ(0u, restriction.size());
		AssertCannotGetValue(restriction, 111);
		EXPECT_EQ(std::set<uint64_t>(), restriction.keys());
	}

	TEST(TEST_CLASS, CanSetMultipleValues) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));

		// Act:
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });
		restriction.set(321, { TokenId(2), 987, model::TokenRestrictionType::GT });
		restriction.set(222, { TokenId(3), 567, model::TokenRestrictionType::EQ });

		// Assert:
		EXPECT_EQ(3u, restriction.size());
		AssertCanGetValue(restriction, 111, 444);
		AssertCanGetValue(restriction, 222, 567);
		AssertCanGetValue(restriction, 321, 987);
		EXPECT_EQ(std::set<uint64_t>({ 111, 222, 321 }), restriction.keys());
	}

	TEST(TEST_CLASS, CanChangeSingleValue) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });
		restriction.set(321, { TokenId(2), 987, model::TokenRestrictionType::GT });

		// Act:
		restriction.set(111, { TokenId(9), 555, model::TokenRestrictionType::EQ });

		// Assert:
		EXPECT_EQ(2u, restriction.size());
		AssertCanGetValue(restriction, 111, 555);
		AssertCanGetValue(restriction, 321, 987);
		EXPECT_EQ(std::set<uint64_t>({ 111, 321 }), restriction.keys());
	}

	TEST(TEST_CLASS, CanRemoveSingleValue) {
		// Arrange:
		TokenGlobalRestriction restriction(TokenId(123));
		restriction.set(111, { TokenId(17), 444, model::TokenRestrictionType::LT });
		restriction.set(321, { TokenId(2), 987, model::TokenRestrictionType::GT });

		// Act:
		restriction.set(111, { TokenId(9), 555, model::TokenRestrictionType::NONE });

		// Assert:
		EXPECT_EQ(1u, restriction.size());
		AssertCannotGetValue(restriction, 111);
		AssertCanGetValue(restriction, 321, 987);
		EXPECT_EQ(std::set<uint64_t>({ 321 }), restriction.keys());
	}

	// endregion
}}
