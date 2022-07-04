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

#include "src/validators/Validators.h"
#include "tests/test/TokenRestrictionCacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRestrictionRequiredValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenRestrictionRequired,)

	// region traits

	namespace {
		class TestContext {
		public:
			using NotificationType = model::TokenRestrictionRequiredNotification;

		public:
			TestContext() : m_tokenId(test::GenerateRandomValue<TokenId>())
			{}

		public:
			NotificationType createNotification(uint64_t key) {
				return NotificationType(test::UnresolveXor(m_tokenId), key);
			}

			void addRestrictionWithValueToCache(cache::TokenRestrictionCacheDelta& restrictionCache, uint64_t key) {
				state::TokenGlobalRestriction restriction(m_tokenId);
				restriction.set(key, { TokenId(), 246, model::TokenRestrictionType::EQ });
				restrictionCache.insert(state::TokenRestrictionEntry(restriction));
			}

		private:
			TokenId m_tokenId;
		};
	}

	// endregion

	// region tests

	TEST(TEST_CLASS, FailureWhenCacheDoesNotContainEntry) {
		// Arrange:
		TestContext context;

		auto pValidator = CreateTokenRestrictionRequiredValidator();
		auto notification = context.createNotification(123);
		auto cache = test::TokenRestrictionCacheFactory::Create();

		// Act:
		auto result = test::ValidateNotification(*pValidator, notification, cache);

		// Assert:
		EXPECT_EQ(Failure_RestrictionToken_Unknown_Global_Restriction, result);
	}

	namespace {
		void RunCacheEntryTest(ValidationResult expectedResult, uint64_t notificationKey, uint64_t cacheKey) {
			// Arrange:
			TestContext context;

			auto pValidator = CreateTokenRestrictionRequiredValidator();
			auto notification = context.createNotification(notificationKey);
			auto cache = test::TokenRestrictionCacheFactory::Create();
			{
				auto delta = cache.createDelta();
				context.addRestrictionWithValueToCache(delta.sub<cache::TokenRestrictionCache>(), cacheKey);
				cache.commit(Height());
			}

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	TEST(TEST_CLASS, FailureWhenCacheContainsEntryButNotRule) {
		RunCacheEntryTest(Failure_RestrictionToken_Unknown_Global_Restriction, 123, 124);
	}

	TEST(TEST_CLASS, SuccessWhenCacheContainsEntryAndRule) {
		RunCacheEntryTest(ValidationResult::Success, 123, 123);
	}

	// endregion
}}
