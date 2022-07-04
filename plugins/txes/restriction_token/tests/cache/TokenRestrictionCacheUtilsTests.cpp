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

#include "src/cache/TokenRestrictionCacheUtils.h"
#include "src/cache/TokenRestrictionCache.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS TokenRestrictionCacheUtilsTests

	// region GetTokenGlobalRestrictionResolvedRules

	namespace {
		void AssertResolvedRules(
				const std::vector<TokenRestrictionResolvedRule>& expected,
				const std::vector<TokenRestrictionResolvedRule>& actual) {
			// Assert:
			ASSERT_EQ(expected.size(), actual.size());

			for (auto i = 0u; i < expected.size(); ++i) {
				auto message = "rule at " + std::to_string(i);
				EXPECT_EQ(expected[i].TokenId, actual[i].TokenId) << message;
				EXPECT_EQ(expected[i].RestrictionKey, actual[i].RestrictionKey) << message;
				EXPECT_EQ(expected[i].RestrictionValue, actual[i].RestrictionValue) << message;
				EXPECT_EQ(expected[i].RestrictionType, actual[i].RestrictionType) << message;
			}
		}

		void SeedCacheWithTokenGlobalRestrictionRules(
				TokenRestrictionCacheDelta& delta,
				uint64_t referenceKey,
				TokenId referenceTokenId,
				TokenId secondReferenceTokenId) {
			auto restriction1 = state::TokenGlobalRestriction(TokenId(222));
			restriction1.set(200, { secondReferenceTokenId, 111, model::TokenRestrictionType::NE });
			delta.insert(state::TokenRestrictionEntry(restriction1));

			auto restriction2 = state::TokenGlobalRestriction(TokenId(111));
			restriction2.set(100, { TokenId(), 999, model::TokenRestrictionType::LT });
			restriction2.set(referenceKey, { referenceTokenId, 888, model::TokenRestrictionType::EQ });
			restriction2.set(300, { TokenId(), 777, model::TokenRestrictionType::GE });
			delta.insert(state::TokenRestrictionEntry(restriction2));
		}
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_ResolutionSkippedWhenNoRulesAreSet) {
		// Arrange:
		TokenRestrictionCache cache(CacheConfiguration(), static_cast<model::NetworkIdentifier>(12));
		auto delta = cache.createDelta();
		SeedCacheWithTokenGlobalRestrictionRules(*delta, 200, TokenId(), TokenId());

		// Act:
		std::vector<TokenRestrictionResolvedRule> resolvedRules;
		auto result = GetTokenGlobalRestrictionResolvedRules(delta->asReadOnly(), TokenId(123), resolvedRules);

		// Assert:
		EXPECT_EQ(TokenGlobalRestrictionRuleResolutionResult::No_Rules, result);
	}

	namespace {
		void AssertCanResolveTokenGlobalRestrictionRule(
				uint64_t referenceKey,
				TokenId referenceTokenId,
				TokenId secondReferenceTokenId,
				const TokenRestrictionResolvedRule& expectedSecondResolvedRule) {
			// Arrange:
			TokenRestrictionCache cache(CacheConfiguration(), static_cast<model::NetworkIdentifier>(12));
			auto delta = cache.createDelta();
			SeedCacheWithTokenGlobalRestrictionRules(*delta, referenceKey, referenceTokenId, secondReferenceTokenId);

			// Act:
			std::vector<TokenRestrictionResolvedRule> resolvedRules;
			auto result = GetTokenGlobalRestrictionResolvedRules(delta->asReadOnly(), TokenId(111), resolvedRules);

			// Assert:
			EXPECT_EQ(TokenGlobalRestrictionRuleResolutionResult::Success, result);

			auto expectedResolvedRules = std::vector<TokenRestrictionResolvedRule>{
				{ TokenId(111), 100, 999, model::TokenRestrictionType::LT },
				expectedSecondResolvedRule,
				{ TokenId(111), 300, 777, model::TokenRestrictionType::GE }
			};
			AssertResolvedRules(expectedResolvedRules, resolvedRules);
		}
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CanResolveRuleWithImplicitSelfReference) {
		// Assert: token restriction entry 111 is in the cache
		AssertCanResolveTokenGlobalRestrictionRule(200, TokenId(), TokenId(), {
			TokenId(111), 200, 888, model::TokenRestrictionType::EQ
		});
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CanResolveRuleWithSingleLevelReference) {
		// Assert: token restriction entry 222 is in the cache
		AssertCanResolveTokenGlobalRestrictionRule(200, TokenId(222), TokenId(), {
			TokenId(222), 200, 888, model::TokenRestrictionType::EQ
		});
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CanResolveRuleReferencingNonexistentEntry) {
		// Assert: token restriction entry 333 is not in the cache
		AssertCanResolveTokenGlobalRestrictionRule(200, TokenId(333), TokenId(), {
			TokenId(333), 200, 888, model::TokenRestrictionType::EQ
		});
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CanResolveRuleReferencingEntryWithNonexistentKey) {
		// Assert: token restriction entry 222 is in the cache but it doesn't have a rule with key 201
		AssertCanResolveTokenGlobalRestrictionRule(201, TokenId(222), TokenId(), {
			TokenId(222), 201, 888, model::TokenRestrictionType::EQ
		});
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CanResolveRuleWithMultiLevelReference) {
		// Assert: token restriction entry reference loop is ignored supported
		AssertCanResolveTokenGlobalRestrictionRule(200, TokenId(222), TokenId(111), {
			TokenId(222), 200, 888, model::TokenRestrictionType::EQ
		});
	}

	TEST(TEST_CLASS, GetTokenGlobalRestrictionResolvedRules_CannotResolveRuleWithExplicitSelfReference) {
		// Arrange:
		TokenRestrictionCache cache(CacheConfiguration(), static_cast<model::NetworkIdentifier>(12));
		auto delta = cache.createDelta();
		SeedCacheWithTokenGlobalRestrictionRules(*delta, 200, TokenId(111), TokenId());

		// Act:
		std::vector<TokenRestrictionResolvedRule> resolvedRules;
		auto result = GetTokenGlobalRestrictionResolvedRules(delta->asReadOnly(), TokenId(111), resolvedRules);

		// Assert: token restriction entry cannot reference itself
		EXPECT_EQ(TokenGlobalRestrictionRuleResolutionResult::Invalid_Rule, result);
	}

	// endregion

	// region EvaluateTokenRestrictionResolvedRulesForAddress

	namespace {
		void SeedCacheWithTokenAddressRestrictionRules(TokenRestrictionCacheDelta& delta, const Address address) {
			auto restriction1 = state::TokenAddressRestriction(TokenId(222), address);
			restriction1.set(200, 111);
			delta.insert(state::TokenRestrictionEntry(restriction1));

			auto restriction2 = state::TokenAddressRestriction(TokenId(111), address);
			restriction2.set(100, 998);
			restriction2.set(200, 888);
			restriction2.set(300, 778);
			delta.insert(state::TokenRestrictionEntry(restriction2));
		}

		void AssertTokenRestrictionRulesForAddressEvaluation(
				bool expectedResult,
				const std::vector<TokenRestrictionResolvedRule>& resolvedRules) {
			// Arrange:
			auto address = test::GenerateRandomByteArray<Address>();
			TokenRestrictionCache cache(CacheConfiguration(), static_cast<model::NetworkIdentifier>(12));
			auto delta = cache.createDelta();
			SeedCacheWithTokenAddressRestrictionRules(*delta, address);

			// Act:
			auto result = EvaluateTokenRestrictionResolvedRulesForAddress(delta->asReadOnly(), address, resolvedRules);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	TEST(TEST_CLASS, EvaluateTokenRestrictionResolvedRulesForAddress_SuccessWhenNoRulesSpecified) {
		AssertTokenRestrictionRulesForAddressEvaluation(true, {});
	}

	TEST(TEST_CLASS, EvaluateTokenRestrictionResolvedRulesForAddress_SuccessWhenAllRulesFromSingleEntryPass) {
		AssertTokenRestrictionRulesForAddressEvaluation(true, {
			{ TokenId(111), 100, 999, model::TokenRestrictionType::LT },
			{ TokenId(111), 200, 888, model::TokenRestrictionType::EQ },
			{ TokenId(111), 300, 777, model::TokenRestrictionType::GE }
		});
	}

	TEST(TEST_CLASS, EvaluateTokenRestrictionResolvedRulesForAddress_SuccessWhenAllRulesFromMultipleEntriesPass) {
		AssertTokenRestrictionRulesForAddressEvaluation(true, {
			{ TokenId(111), 100, 999, model::TokenRestrictionType::LT },
			{ TokenId(222), 200, 111, model::TokenRestrictionType::EQ },
			{ TokenId(111), 300, 777, model::TokenRestrictionType::GE }
		});
	}

	TEST(TEST_CLASS, EvaluateTokenRestrictionResolvedRulesForAddress_FailureWhenAnyRuleFails) {
		AssertTokenRestrictionRulesForAddressEvaluation(false, {
			{ TokenId(111), 100, 999, model::TokenRestrictionType::LT },
			{ TokenId(111), 200, 887, model::TokenRestrictionType::EQ },
			{ TokenId(111), 300, 777, model::TokenRestrictionType::GE }
		});
	}

	TEST(TEST_CLASS, EvaluateTokenRestrictionResolvedRulesForAddress_FailureWhenRequiredRuleDoesNotExist) {
		AssertTokenRestrictionRulesForAddressEvaluation(false, {
			{ TokenId(111), 100, 999, model::TokenRestrictionType::LT },
			{ TokenId(111), 201, 888, model::TokenRestrictionType::EQ },
			{ TokenId(111), 300, 777, model::TokenRestrictionType::GE }
		});
	}

	// endregion
}}
