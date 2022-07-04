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
#include "src/cache/TokenCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace observers {

#define TEST_CLASS TokenDefinitionObserverTests

	using ObserverTestContext = test::ObserverTestContextT<test::TokenCacheFactory>;

	DEFINE_COMMON_OBSERVER_TESTS(TokenDefinition,)

	namespace {
		constexpr TokenId Default_Token_Id(345);
		constexpr Height Seed_Height(7);

		model::TokenDefinitionNotification CreateNotification(const Address& owner, const model::TokenProperties& properties) {
			return model::TokenDefinitionNotification(owner, Default_Token_Id, properties);
		}

		template<typename TSeedCacheFunc, typename TCheckCacheFunc>
		void RunTest(
				const model::TokenDefinitionNotification& notification,
				ObserverTestContext&& context,
				TSeedCacheFunc seedCache,
				TCheckCacheFunc checkCache) {
			// Arrange:
			auto pObserver = CreateTokenDefinitionObserver();

			auto& tokenCacheDelta = context.cache().sub<cache::TokenCache>();
			seedCache(tokenCacheDelta);

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert: check the cache
			checkCache(tokenCacheDelta);
		}

		void SeedCacheEmpty(cache::TokenCacheDelta& tokenCacheDelta) {
			// Sanity:
			test::AssertCacheContents(tokenCacheDelta, {});
		}

		void SeedCacheWithDefaultToken(cache::TokenCacheDelta& tokenCacheDelta) {
			auto definition = state::TokenDefinition(Seed_Height, Address(), 1, test::CreateTokenPropertiesFromValues(1, 2, 20));
			tokenCacheDelta.insert(state::TokenEntry(Default_Token_Id, definition));

			// Sanity:
			test::AssertCacheContents(tokenCacheDelta, { Default_Token_Id.unwrap() });
		}

		void AssertDefaultToken(
				const cache::TokenCacheDelta& tokenCacheDelta,
				const Address& owner,
				Height height,
				uint32_t expectedRevision,
				const model::TokenProperties& expectedProperties) {
			// Assert: the token was added
			ASSERT_TRUE(tokenCacheDelta.contains(Default_Token_Id));

			// - entry
			const auto& entry = tokenCacheDelta.find(Default_Token_Id).get();
			EXPECT_EQ(Default_Token_Id, entry.tokenId());

			// - definition
			const auto& definition = entry.definition();
			EXPECT_EQ(height, definition.startHeight());
			EXPECT_EQ(owner, definition.ownerAddress());
			EXPECT_EQ(expectedRevision, definition.revision());
			EXPECT_EQ(expectedProperties, definition.properties());

			// - supply
			EXPECT_EQ(Amount(), entry.supply());
		}
	}

	// region commit

	TEST(TEST_CLASS, ObserverAddsTokenOnCommit) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(3, 6, 15);
		auto notification = CreateNotification(owner, properties);

		// Act: add it
		auto context = ObserverTestContext(NotifyMode::Commit, Height(888));
		RunTest(notification, std::move(context), SeedCacheEmpty, [&owner](const auto& tokenCacheDelta) {
			// Assert: the token was added
			EXPECT_EQ(1u, tokenCacheDelta.size());
			AssertDefaultToken(tokenCacheDelta, owner, Height(888), 1, test::CreateTokenPropertiesFromValues(3, 6, 15));
		});
	}

	TEST(TEST_CLASS, ObserverOverwritesTokenOnCommit) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(3, 6, 15);
		auto notification = CreateNotification(owner, properties);

		// Act: add it
		auto context = ObserverTestContext(NotifyMode::Commit, Height(888));
		RunTest(notification, std::move(context), SeedCacheWithDefaultToken, [&owner](const auto& tokenCacheDelta) {
			// Assert: the token definition was changed
			// - height did not change
			// - xor properties were xored (1 ^ 3, 2 ^ 6)
			// - duration was added
			EXPECT_EQ(1u, tokenCacheDelta.size());
			AssertDefaultToken(tokenCacheDelta, owner, Seed_Height, 2, test::CreateTokenPropertiesFromValues(2, 4, 20 + 15));
		});
	}

	// endregion

	// region rollback

	namespace {
		void AddTwoTokens(cache::TokenCacheDelta& tokenCacheDelta, uint32_t revision) {
			auto properties = test::CreateTokenPropertiesFromValues(2, 4, 20 + 15);
			auto definition = state::TokenDefinition(Seed_Height, Address(), revision, properties);
			for (auto id : { Default_Token_Id, TokenId(987) })
				tokenCacheDelta.insert(state::TokenEntry(id, definition));

			// Sanity:
			test::AssertCacheContents(tokenCacheDelta, { Default_Token_Id.unwrap(), 987 });
		}
	}

	TEST(TEST_CLASS, ObserverRemovesTokenOnRollbackWhenObserverDefinitionCounterIsEqualToOne) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(3, 6, 15);
		auto notification = CreateNotification(owner, properties);
		auto seedTokens = [](auto& tokenCacheDelta) { AddTwoTokens(tokenCacheDelta, 1); };

		// Act: remove it
		auto context = ObserverTestContext(NotifyMode::Rollback, Seed_Height);
		RunTest(notification, std::move(context), seedTokens, [](const auto& tokenCacheDelta) {
			// Assert: the token was removed
			EXPECT_EQ(1u, tokenCacheDelta.size());
			EXPECT_FALSE(tokenCacheDelta.contains(Default_Token_Id));
			EXPECT_TRUE(tokenCacheDelta.contains(TokenId(987)));
		});
	}

	TEST(TEST_CLASS, ObserverDoesNotRemoveTokenOnRollbackWhenDefinitionCounterIsGreaterThanOne) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(3, 6, 15);
		auto notification = CreateNotification(owner, properties);
		auto seedTokens = [](auto& tokenCacheDelta) { AddTwoTokens(tokenCacheDelta, 2); };

		// Act: remove it
		auto context = ObserverTestContext(NotifyMode::Rollback, Seed_Height);
		RunTest(notification, std::move(context), seedTokens, [&owner](const auto& tokenCacheDelta) {
			// Assert: the token was removed
			EXPECT_EQ(2u, tokenCacheDelta.size());
			EXPECT_TRUE(tokenCacheDelta.contains(Default_Token_Id));
			EXPECT_TRUE(tokenCacheDelta.contains(TokenId(987)));

			// Assert: the default token's definition was changed
			// - height did not change
			// - xor properties were xored (2 ^ 3, 4 ^ 6)
			// - duration was subtracted
			AssertDefaultToken(tokenCacheDelta, owner, Seed_Height, 1, test::CreateTokenPropertiesFromValues(1, 2, 20));
		});
	}

	// endregion
}}
