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

#include "src/state/TokenGlobalRestrictionNotificationFacade.h"
#include "bitxorcore/crypto/Hashes.h"
#include "tests/test/core/ResolverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenGlobalRestrictionNotificationFacadeTests

	// region test utils

	namespace {
		using FacadeType = TokenGlobalRestrictionNotificationFacade<static_cast<model::NotificationType>(0)>;

		auto CreateNotification(
				TokenId tokenId,
				TokenId referenceTokenId,
				uint64_t key,
				uint64_t value,
				model::TokenRestrictionType restrictionType = model::TokenRestrictionType::NONE) {
			return FacadeType::NotificationType(
					test::UnresolveXor(tokenId),
					test::UnresolveXor(referenceTokenId),
					key,
					value,
					restrictionType);
		}
	}

	// endregion

	// region uniqueKey

	TEST(TEST_CLASS, UniqueKeyConstructsKeyFromNotification) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto uniqueKey = facade.uniqueKey();

		// Assert:
		Hash256 expectedUniqueKey;
		crypto::Sha3_256_Builder builder;
		builder.update({ reinterpret_cast<const uint8_t*>(&tokenId), sizeof(TokenId) });
		builder.update(Address());
		builder.final(expectedUniqueKey);

		EXPECT_EQ(expectedUniqueKey, uniqueKey);
	}

	// endregion

	// region isDeleteAction

	namespace {
		bool IsDeleteAction(model::TokenRestrictionType restrictionType) {
			// Arrange:
			auto tokenId = test::GenerateRandomValue<TokenId>();
			auto referenceTokenId = test::GenerateRandomValue<TokenId>();
			auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, restrictionType);
			auto resolvers = test::CreateResolverContextXor();
			auto facade = FacadeType(notification, resolvers);

			// Act:
			return facade.isDeleteAction();
		}
	}

	TEST(TEST_CLASS, IsDeleteActionReturnsTrueWhenRestrictionTypeIsNone) {
		EXPECT_TRUE(IsDeleteAction(model::TokenRestrictionType::NONE));
	}

	TEST(TEST_CLASS, IsDeleteActionReturnsFalseWhenRestrictionTypeIsNotNone) {
		for (auto i = 1u; i <= 0xFF; ++i)
			EXPECT_FALSE(IsDeleteAction(static_cast<model::TokenRestrictionType>(i))) << i;
	}

	// endregion

	// region tryGet

	TEST(TEST_CLASS, TryGetReturnsFalseWhenEntryDoesNotContainRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenGlobalRestriction(tokenId);
		restriction.set(112, { TokenId(987), 444, model::TokenRestrictionType::LT });
		auto entry = TokenRestrictionEntry(restriction);

		// Act:
		TokenGlobalRestriction::RestrictionRule rule;
		auto result = facade.tryGet(entry, rule);

		// Assert:
		EXPECT_FALSE(result);
	}

	TEST(TEST_CLASS, TryGetReturnsTrueWhenEntryContainsRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenGlobalRestriction(tokenId);
		restriction.set(111, { TokenId(987), 444, model::TokenRestrictionType::LT });
		auto entry = TokenRestrictionEntry(restriction);

		// Act:
		TokenGlobalRestriction::RestrictionRule rule;
		auto result = facade.tryGet(entry, rule);

		// Assert:
		EXPECT_TRUE(result);
		EXPECT_EQ(TokenId(987), rule.ReferenceTokenId);
		EXPECT_EQ(444u, rule.RestrictionValue);
		EXPECT_EQ(model::TokenRestrictionType::LT, rule.RestrictionType);
	}

	// endregion

	// region update

	TEST(TEST_CLASS, UpdateSetsRepresentativeRuleInEntry) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::GT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		auto restriction = TokenGlobalRestriction(tokenId);
		restriction.set(112, { TokenId(987), 444, model::TokenRestrictionType::LT });
		auto entry = TokenRestrictionEntry(restriction);

		// Sanity:
		EXPECT_EQ(1u, entry.asGlobalRestriction().size());

		// Act:
		auto result = facade.update(entry);

		// Assert:
		EXPECT_EQ(2u, result);
		EXPECT_EQ(2u, entry.asGlobalRestriction().size());

		TokenGlobalRestriction::RestrictionRule rule;
		entry.asGlobalRestriction().tryGet(111, rule);
		EXPECT_EQ(referenceTokenId, rule.ReferenceTokenId);
		EXPECT_EQ(222u, rule.RestrictionValue);
		EXPECT_EQ(model::TokenRestrictionType::GT, rule.RestrictionType);
	}

	// endregion

	// region isUnset

	TEST(TEST_CLASS, IsUnsetReturnsTrueWhenAllValuesAreUnset) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 0);
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
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isUnset = facade.isUnset();

		// Assert:
		EXPECT_FALSE(isUnset);
	}

	TEST(TEST_CLASS, IsUnsetReturnsFalseWhenRestrictionTypeIsSet) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 0, model::TokenRestrictionType::EQ);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isUnset = facade.isUnset();

		// Assert:
		EXPECT_FALSE(isUnset);
	}

	// endregion

	// region isMatch

	TEST(TEST_CLASS, IsMatchReturnsTrueWhenAllValuesMatch) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::LT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch({ referenceTokenId, 222, model::TokenRestrictionType::LT });

		// Assert:
		EXPECT_TRUE(isMatch);
	}

	TEST(TEST_CLASS, IsMatchReturnsFalseWhenReferenceTokenIdIsDifferent) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::LT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch({ test::GenerateRandomValue<TokenId>(), 222, model::TokenRestrictionType::LT });

		// Assert:
		EXPECT_FALSE(isMatch);
	}

	TEST(TEST_CLASS, IsMatchReturnsFalseWhenRestrictionValueIsDifferent) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::LT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch({ referenceTokenId, 223, model::TokenRestrictionType::LT });

		// Assert:
		EXPECT_FALSE(isMatch);
	}

	TEST(TEST_CLASS, IsMatchReturnsFalseWhenRestrictionTypeIsDifferent) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::LT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto isMatch = facade.isMatch({ referenceTokenId, 222, model::TokenRestrictionType::LE });

		// Assert:
		EXPECT_FALSE(isMatch);
	}

	// endregion

	// region toRule

	TEST(TEST_CLASS, ToRuleReturnsRepresentativeRule) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222, model::TokenRestrictionType::LT);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto rule = facade.toRule();

		// Assert:
		EXPECT_EQ(referenceTokenId, rule.ReferenceTokenId);
		EXPECT_EQ(222u, rule.RestrictionValue);
		EXPECT_EQ(model::TokenRestrictionType::LT, rule.RestrictionType);
	}

	// endregion

	// region toRestriction

	TEST(TEST_CLASS, ToRestrictionReturnsRepresentativeRestriction) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto referenceTokenId = test::GenerateRandomValue<TokenId>();
		auto notification = CreateNotification(tokenId, referenceTokenId, 111, 222);
		auto resolvers = test::CreateResolverContextXor();
		auto facade = FacadeType(notification, resolvers);

		// Act:
		auto restriction = facade.toRestriction();

		// Assert:
		EXPECT_EQ(tokenId, restriction.tokenId());
		EXPECT_EQ(0u, restriction.size());
	}

	// endregion
}}
