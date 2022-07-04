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

#include "src/observers/Observers.h"
#include "tests/test/TokenRestrictionCacheTestUtils.h"
#include "tests/test/TokenRestrictionTestTraits.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace observers {

#define TEST_CLASS TokenRestrictionModificationObserverTests

	DEFINE_COMMON_OBSERVER_TESTS(TokenGlobalRestrictionCommitModification,)
	DEFINE_COMMON_OBSERVER_TESTS(TokenGlobalRestrictionRollbackModification,)

	DEFINE_COMMON_OBSERVER_TESTS(TokenAddressRestrictionCommitModification,)
	DEFINE_COMMON_OBSERVER_TESTS(TokenAddressRestrictionRollbackModification,)

	// region traits

	namespace {
		using ObserverTestContext = test::ObserverTestContextT<test::TokenRestrictionCacheFactory>;

		enum class InitializationScheme { Unset, Set };

		template<typename TTestTraits>
		class BasicTraits : public TTestTraits {
		public:
			typename TTestTraits::NotificationType createNotification(uint64_t key, uint64_t value, InitializationScheme scheme) {
				if (InitializationScheme::Unset == scheme)
					return this->createDeleteNotification(key);
				else
					return this->createAddNotification(key, value);
			}

			void addRestrictionWithValueToCache(cache::TokenRestrictionCacheDelta& restrictionCache, uint64_t key, uint64_t value) {
				this->addRestrictionWithValuesToCache(restrictionCache, { std::make_pair(key, value) });
			}

		private:
			TTestTraits m_traits;
		};

		using GlobalCommitNotification = model::TokenGlobalRestrictionModificationNewValueNotification;
		using GlobalRollbackNotification = model::TokenGlobalRestrictionModificationPreviousValueNotification;
		using AddressCommitNotification = model::TokenAddressRestrictionModificationNewValueNotification;
		using AddressRollbackNotification = model::TokenAddressRestrictionModificationPreviousValueNotification;

		class GlobalCommitTraits : public BasicTraits<test::TokenGlobalRestrictionTestTraits<GlobalCommitNotification>> {
		public:
			static constexpr auto Execute_Mode = NotifyMode::Commit;
			static constexpr auto Bypass_Mode = NotifyMode::Rollback;

			static constexpr auto CreateObserver = CreateTokenGlobalRestrictionCommitModificationObserver;
		};

		class GlobalRollbackTraits : public BasicTraits<test::TokenGlobalRestrictionTestTraits<GlobalRollbackNotification>> {
		public:
			static constexpr auto Execute_Mode = NotifyMode::Rollback;
			static constexpr auto Bypass_Mode = NotifyMode::Commit;

			static constexpr auto CreateObserver = CreateTokenGlobalRestrictionRollbackModificationObserver;
		};

		class AddressCommitTraits : public BasicTraits<test::TokenAddressRestrictionTestTraits<AddressCommitNotification>> {
		public:
			static constexpr auto Execute_Mode = NotifyMode::Commit;
			static constexpr auto Bypass_Mode = NotifyMode::Rollback;

			static constexpr auto CreateObserver = CreateTokenAddressRestrictionCommitModificationObserver;
		};

		class AddressRollbackTraits : public BasicTraits<test::TokenAddressRestrictionTestTraits<AddressRollbackNotification>> {
		public:
			static constexpr auto Execute_Mode = NotifyMode::Rollback;
			static constexpr auto Bypass_Mode = NotifyMode::Commit;

			static constexpr auto CreateObserver = CreateTokenAddressRestrictionRollbackModificationObserver;
		};
	}

#define RESTRICTION_TYPE_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TokenGlobalRestrictionCommitModificationObserverTests, TEST_NAME) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<GlobalCommitTraits>(); \
	} \
	TEST(TokenGlobalRestrictionRollbackModificationObserverTests, TEST_NAME) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<GlobalRollbackTraits>(); \
	} \
	TEST(TokenAddressRestrictionCommitModificationObserverTests, TEST_NAME) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AddressCommitTraits>(); \
	} \
	TEST(TokenAddressRestrictionRollbackModificationObserverTests, TEST_NAME) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AddressRollbackTraits>(); \
	} \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// endregion

	// region cache does not contain entry

	RESTRICTION_TYPE_BASED_TEST(OppositeNotificationModeBypassesChangesWhenCacheDoesNotContainEntry) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Bypass_Mode);
		const auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);

		// Sanity:
		EXPECT_EQ(0u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(0u, restrictionCache.size());
	}

	RESTRICTION_TYPE_BASED_TEST(CanAddNewRestrictionWhenCacheDoesNotContainEntry) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Execute_Mode);
		const auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);

		// Sanity:
		EXPECT_EQ(0u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(123, 456) });
	}

	// endregion

	// region cache contains entry but not rule

	RESTRICTION_TYPE_BASED_TEST(OppositeNotificationModeBypassesChangesWhenCacheContainsEntryButNotRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Bypass_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);
		traits.addRestrictionWithValueToCache(restrictionCache, 234, 888);

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(234, 888) });
	}

	RESTRICTION_TYPE_BASED_TEST(CanAddNewRestrictionWhenCacheContainsEntryButNotRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Execute_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);
		traits.addRestrictionWithValueToCache(restrictionCache, 234, 888);

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(234, 888), std::make_pair(123, 456) });
	}

	// endregion

	// region cache contains entry and rule

	RESTRICTION_TYPE_BASED_TEST(OppositeNotificationModeBypassesChangesWhenCacheContainsEntryAndRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Bypass_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);
		traits.addRestrictionWithValueToCache(restrictionCache, 123, 111);

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(123, 111) });
	}

	RESTRICTION_TYPE_BASED_TEST(CanModifyExistingRestrictionWhenCacheContainsEntryAndRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Execute_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 456, InitializationScheme::Set);
		traits.addRestrictionWithValueToCache(restrictionCache, 123, 111);

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(123, 456) });
	}

	RESTRICTION_TYPE_BASED_TEST(CanDeleteExistingRestrictionWhenCacheContainsEntryAndRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Execute_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 0, InitializationScheme::Unset);
		traits.addRestrictionWithValuesToCache(restrictionCache, { std::make_pair(123, 111), std::make_pair(246, 222) });

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(1u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		ASSERT_TRUE(!!restrictionEntryIter.tryGet());
		traits.assertEqual(restrictionEntryIter.get(), { std::make_pair(246, 222) });
	}

	RESTRICTION_TYPE_BASED_TEST(CanDeleteExistingEntryWhenCacheContainsEntryAndRule) {
		// Arrange:
		TTraits traits;
		ObserverTestContext context(TTraits::Execute_Mode);
		auto& restrictionCache = context.cache().sub<cache::TokenRestrictionCache>();

		auto pObserver = TTraits::CreateObserver();
		auto notification = traits.createNotification(123, 0, InitializationScheme::Unset);
		traits.addRestrictionWithValueToCache(restrictionCache, 123, 111);

		// Sanity:
		EXPECT_EQ(1u, restrictionCache.size());

		// Act:
		test::ObserveNotification(*pObserver, notification, context);

		// Assert:
		EXPECT_EQ(0u, restrictionCache.size());

		auto restrictionEntryIter = restrictionCache.find(traits.uniqueKey());
		EXPECT_FALSE(!!restrictionEntryIter.tryGet());
	}

	// endregion
}}
