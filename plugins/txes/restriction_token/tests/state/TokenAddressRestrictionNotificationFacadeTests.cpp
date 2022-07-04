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

#include "src/state/TokenAddressRestrictionNotificationFacade.h"
#include "bitxorcore/crypto/Hashes.h"
#include "tests/test/core/ResolverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenAddressRestrictionNotificationFacadeTests

	// region test utils

	namespace {
		using FacadeType = TokenAddressRestrictionNotificationFacade<static_cast<model::NotificationType>(0)>;

		auto CreateNotification(TokenId tokenId, const UnresolvedAddress& targetAddress, uint64_t key, uint64_t value) {
			return FacadeType::NotificationType(test::UnresolveXor(tokenId), key, targetAddress, value);
		}
	}

	// endregion

	// region uniqueKey

	TEST(TEST_CLASS, UniqueKeyConstructsKeyFromNotification) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();
		auto unresolvedAddress = test::UnresolveXor(address);
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto uniqueKey = facade.uniqueKey();

		// Assert:
		Hash256 expectedUniqueKey;
		crypto::Sha3_256_Builder builder;
		builder.update({ reinterpret_cast<const uint8_t*>(&tokenId), sizeof(TokenId) });
		builder.update(address);
		builder.final(expectedUniqueKey);

		EXPECT_EQ(expectedUniqueKey, uniqueKey);
	}

	// endregion

	// region isDeleteAction

	namespace {
		bool IsDeleteAction(uint64_t value) {
			// Arrange:
			auto tokenId = test::GenerateRandomValue<TokenId>();
			auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
			auto notification = CreateNotification(tokenId, unresolvedAddress, 111, value);
			auto resolvers = test::CreateResolverContextXor();
			auto facade = FacadeType(notification, resolvers);

			// Act:
			return facade.isDeleteAction();
		}
	}

	TEST(TEST_CLASS, IsDeleteActionReturnsTrueWhenRestrictionValueIsSentinel) {
		EXPECT_TRUE(IsDeleteAction(TokenAddressRestriction::Sentinel_Removal_Value));
	}

	TEST(TEST_CLASS, IsDeleteActionReturnsFalseWhenRestrictionValueIsNotSentinel) {
		// Assert: try a subset of values
		for (auto i = 0u; i <= 0xFF; ++i)
			EXPECT_FALSE(IsDeleteAction(i)) << i;
	}

	// endregion

	// region tryGet

	TEST(TEST_CLASS, TryGetReturnsFalseWhenEntryDoesNotContainRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();
		auto unresolvedAddress = test::UnresolveXor(address);
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenAddressRestriction(tokenId, address);
		restriction.set(112, 444);
		auto entry = TokenRestrictionEntry(restriction);

		// Act:
		uint64_t rule;
		auto result = facade.tryGet(entry, rule);

		// Assert:
		EXPECT_FALSE(result);
	}

	TEST(TEST_CLASS, TryGetReturnsTrueWhenEntryContainsRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();
		auto unresolvedAddress = test::UnresolveXor(address);
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenAddressRestriction(tokenId, address);
		restriction.set(111, 444);
		auto entry = TokenRestrictionEntry(restriction);

		// Act:
		uint64_t rule;
		auto result = facade.tryGet(entry, rule);

		// Assert:
		EXPECT_TRUE(result);
		EXPECT_EQ(444u, rule);
	}

	// endregion

	// region update

	TEST(TEST_CLASS, UpdateSetsRepresentativeRuleInEntry) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();
		auto unresolvedAddress = test::UnresolveXor(address);
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenAddressRestriction(tokenId, address);
		restriction.set(112, 444);
		auto entry = TokenRestrictionEntry(restriction);

		// Sanity:
		EXPECT_EQ(1u, entry.asAddressRestriction().size());

		// Act:
		auto result = facade.update(entry);

		// Assert:
		EXPECT_EQ(2u, result);
		EXPECT_EQ(2u, entry.asAddressRestriction().size());

		auto rule = entry.asAddressRestriction().get(111);
		EXPECT_EQ(222u, rule);
	}

	// endregion

	// region isUnset

	TEST(TEST_CLASS, IsUnsetReturnsTrueWhenRestrictionValueIsUnset) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, TokenAddressRestriction::Sentinel_Removal_Value);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isUnset = facade.isUnset();

		// Assert:
		EXPECT_TRUE(isUnset);
	}

	TEST(TEST_CLASS, IsUnsetReturnsFalseWhenRestrictionValueIsSet) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isUnset = facade.isUnset();

		// Assert:
		EXPECT_FALSE(isUnset);
	}

	// endregion

	// region isMatch

	TEST(TEST_CLASS, IsMatchReturnsTrueWhenRestrictionValueMatches) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch(222);

		// Assert:
		EXPECT_TRUE(isMatch);
	}

	TEST(TEST_CLASS, IsMatchReturnsFalseWhenRestrictionValueIsDifferent) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch(223);

		// Assert:
		EXPECT_FALSE(isMatch);
	}

	// endregion

	// region toRule

	TEST(TEST_CLASS, ToRuleReturnsRepresentativeRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto unresolvedAddress = test::GenerateRandomByteArray<UnresolvedAddress>();
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto rule = facade.toRule();

		// Assert:
		EXPECT_EQ(222u, rule);
	}

	// endregion

	// region toRestriction

	TEST(TEST_CLASS, ToRestrictionReturnsRepresentativeRestriction) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();
		auto unresolvedAddress = test::UnresolveXor(address);
		auto notification = CreateNotification(tokenId, unresolvedAddress, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto restriction = facade.toRestriction();

		// Assert:
		EXPECT_EQ(tokenId, restriction.tokenId());
		EXPECT_EQ(address, restriction.address());
		EXPECT_EQ(0u, restriction.size());
	}

	// endregion
}}
