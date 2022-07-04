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

#include "src/state/TokenEntry.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenEntryTests

	// region ctor

	TEST(TEST_CLASS, CanCreateTokenEntry) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));

		// Act:
		auto entry = TokenEntry(TokenId(225), definition);

		// Assert:
		EXPECT_EQ(TokenId(225), entry.tokenId());
		EXPECT_EQ(Height(123), entry.definition().startHeight());
		EXPECT_EQ(Amount(), entry.supply());
	}

	// endregion

	// region supply

	TEST(TEST_CLASS, CanIncreaseSupply) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act:
		entry.increaseSupply(Amount(321));

		// Assert:
		EXPECT_EQ(Amount(432 + 321), entry.supply());
	}

	TEST(TEST_CLASS, CanIncreaseSupplyToMax) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act:
		entry.increaseSupply(Amount(std::numeric_limits<uint64_t>::max() - 432));

		// Assert:
		EXPECT_EQ(Amount(std::numeric_limits<uint64_t>::max()), entry.supply());
	}

	TEST(TEST_CLASS, CannotIncreaseSupplyAboveMax) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act + Assert:
		EXPECT_THROW(entry.increaseSupply(Amount(std::numeric_limits<uint64_t>::max() - 431)), bitxorcore_invalid_argument);
	}

	TEST(TEST_CLASS, CanDecreaseSupply) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act:
		entry.decreaseSupply(Amount(321));

		// Assert:
		EXPECT_EQ(Amount(432 - 321), entry.supply());
	}

	TEST(TEST_CLASS, CanDecreaseSupplyToZero) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act:
		entry.decreaseSupply(Amount(432));

		// Assert:
		EXPECT_EQ(Amount(), entry.supply());
	}

	TEST(TEST_CLASS, CannotDecreaseSupplyBelowZero) {
		// Arrange:
		auto definition = test::CreateTokenDefinition(Height(123));
		auto entry = TokenEntry(TokenId(225), definition);
		entry.increaseSupply(Amount(432));

		// Act + Assert:
		EXPECT_THROW(entry.decreaseSupply(Amount(433)), bitxorcore_invalid_argument);
	}

	// endregion

	// region isActive

	namespace {
		TokenDefinition CreateTokenDefinition(Height height, uint64_t duration) {
			auto owner = test::CreateRandomOwner();
			return TokenDefinition(height, owner, 3, test::CreateTokenPropertiesWithDuration(BlockDuration(duration)));
		}
	}

	TEST(TEST_CLASS, IsActiveReturnsFalseWhenDefinitionIsInactive) {
		// Arrange: definition expires at height 123 + 150 = 273
		auto entry = TokenEntry(TokenId(225), CreateTokenDefinition(Height(123), 150));

		// Act + Assert:
		EXPECT_FALSE(entry.isActive(Height(50)));
		EXPECT_FALSE(entry.isActive(Height(122)));
		EXPECT_FALSE(entry.isActive(Height(273)));
		EXPECT_FALSE(entry.isActive(Height(350)));
	}

	TEST(TEST_CLASS, IsActiveReturnsTrueWhenDefinitionIsActive) {
		// Arrange: definition expires at height 123 + 150 = 273
		auto entry = TokenEntry(TokenId(225), CreateTokenDefinition(Height(123), 150));

		// Act + Assert:
		EXPECT_TRUE(entry.isActive(Height(123)));
		EXPECT_TRUE(entry.isActive(Height(250)));
		EXPECT_TRUE(entry.isActive(Height(272)));
	}

	// endregion
}}
