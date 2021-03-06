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
#include "tests/test/TokenRestrictionTestTraits.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRestrictionModificationValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenGlobalRestrictionModification,)

	DEFINE_COMMON_VALIDATOR_TESTS(TokenAddressRestrictionModification,)

	// region traits

	namespace {
		enum class InitializationScheme { Unset, Match, Other };

		template<typename TTestTraits>
		class BasicTraits {
		public:
			using NotificationType = typename TTestTraits::NotificationType;

		protected:
			const TTestTraits& testTraits() const {
				return m_traits;
			}

		public:
			NotificationType createNotification(uint64_t key, uint64_t value, InitializationScheme scheme) {
				if (InitializationScheme::Unset == scheme)
					return m_traits.createDeleteNotification(key);
				else
					return m_traits.createAddNotification(key, value);
			}

			void addRestrictionWithValueToCache(cache::TokenRestrictionCacheDelta& restrictionCache, uint64_t key, uint64_t value) {
				m_traits.addRestrictionWithValuesToCache(restrictionCache, { std::make_pair(key, value) });
			}

		private:
			TTestTraits m_traits;
		};

		using GlobalTestTraits = test::TokenGlobalRestrictionTestTraits<
			model::TokenGlobalRestrictionModificationPreviousValueNotification>;
		using AddressTestTraits = test::TokenAddressRestrictionTestTraits<
			model::TokenAddressRestrictionModificationPreviousValueNotification>;

		class GlobalTraits : public BasicTraits<GlobalTestTraits> {
		public:
			static constexpr auto CreateValidator = CreateTokenGlobalRestrictionModificationValidator;
		};

		class GlobalTraitsNonzeroReference : public BasicTraits<GlobalTestTraits> {
		public:
			static constexpr auto CreateValidator = CreateTokenGlobalRestrictionModificationValidator;

		public:
			NotificationType createNotification(uint64_t key, uint64_t value, InitializationScheme scheme) {
				// set the reference token id to a nonzero value
				auto notification = BasicTraits<GlobalTestTraits>::createNotification(key, value, scheme);
				notification.ReferenceTokenId = testTraits().referenceTokenId();
				return notification;
			}
		};

		class AddressTraits : public BasicTraits<AddressTestTraits> {
		public:
			static constexpr auto CreateValidator = CreateTokenAddressRestrictionModificationValidator;
		};
	}

#define RESTRICTION_TYPE_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TokenGlobalRestrictionModificationValidatorTests, TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<GlobalTraits>(); } \
	TEST(TokenGlobalRestrictionModificationValidatorTests, TEST_NAME##_NonzeroReference) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<GlobalTraitsNonzeroReference>(); \
	} \
	TEST(TokenAddressRestrictionModificationValidatorTests, TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AddressTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// endregion

	// region cache does not contain entry

	namespace {
		template<typename TTraits>
		void RunCacheDoesNotContainEntryTest(ValidationResult expectedResult, InitializationScheme scheme) {
			// Arrange:
			TTraits traits;

			auto pValidator = TTraits::CreateValidator();
			auto notification = traits.createNotification(123, 444, scheme);
			auto cache = test::TokenRestrictionCacheFactory::Create();

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	RESTRICTION_TYPE_BASED_TEST(SuccessWhenCacheDoesNotContainEntryAndNotificationIsUnset) {
		RunCacheDoesNotContainEntryTest<TTraits>(ValidationResult::Success, InitializationScheme::Unset);
	}

	RESTRICTION_TYPE_BASED_TEST(FailureWhenCacheDoesNotContainEntryAndNotificationIsSet) {
		RunCacheDoesNotContainEntryTest<TTraits>(Failure_RestrictionToken_Previous_Value_Must_Be_Zero, InitializationScheme::Match);
	}

	// endregion

	// region cache contains entry but not rule

	namespace {
		template<typename TTraits>
		void RunCacheContainsEntryButNotRuleTest(ValidationResult expectedResult, InitializationScheme scheme) {
			// Arrange:
			TTraits traits;

			auto pValidator = TTraits::CreateValidator();
			auto notification = traits.createNotification(123, 444, scheme);
			auto cache = test::TokenRestrictionCacheFactory::Create();
			{
				auto delta = cache.createDelta();
				traits.addRestrictionWithValueToCache(delta.sub<cache::TokenRestrictionCache>(), 124, 555);
				cache.commit(Height());
			}

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	RESTRICTION_TYPE_BASED_TEST(SuccessWhenCacheContainsEntryButNotRuleAndNotificationIsUnset) {
		RunCacheContainsEntryButNotRuleTest<TTraits>(ValidationResult::Success, InitializationScheme::Unset);
	}

	RESTRICTION_TYPE_BASED_TEST(SuccessWhenCacheContainsEntryButNotRuleAndNotificationIsSet) {
		RunCacheContainsEntryButNotRuleTest<TTraits>(Failure_RestrictionToken_Previous_Value_Must_Be_Zero, InitializationScheme::Match);
	}

	// endregion

	// region cache contains entry and rule

	namespace {
		template<typename TTraits>
		void RunCacheModificationTest(ValidationResult expectedResult, InitializationScheme scheme) {
			// Arrange:
			TTraits traits;

			auto pValidator = TTraits::CreateValidator();
			auto notification = traits.createNotification(123, 444, scheme);
			auto cache = test::TokenRestrictionCacheFactory::Create();
			{
				uint64_t value = InitializationScheme::Match == scheme ? 444 : 222;

				auto delta = cache.createDelta();
				traits.addRestrictionWithValueToCache(delta.sub<cache::TokenRestrictionCache>(), 123, value);
				cache.commit(Height());
			}

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	RESTRICTION_TYPE_BASED_TEST(SuccessWhenCacheEntryContainsEntryAndRuleAndNotificationIsMatch) {
		RunCacheModificationTest<TTraits>(ValidationResult::Success, InitializationScheme::Match);
	}

	RESTRICTION_TYPE_BASED_TEST(FailureWhenCacheEntryContainsEntryAndRuleAndNotificationIsUnset) {
		RunCacheModificationTest<TTraits>(Failure_RestrictionToken_Previous_Value_Mismatch, InitializationScheme::Unset);
	}

	RESTRICTION_TYPE_BASED_TEST(FailureWhenCacheEntryContainsEntryAndRuleAndNotificationIsOther) {
		RunCacheModificationTest<TTraits>(Failure_RestrictionToken_Previous_Value_Mismatch, InitializationScheme::Other);
	}

	// endregion
}}
