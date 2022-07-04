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

#include "src/cache/TokenCache.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/cache/CacheBasicTests.h"
#include "tests/test/cache/CacheMixinsTests.h"
#include "tests/test/cache/DeltaElementsMixinTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS TokenCacheTests

	// region mixin traits based tests

	namespace {
		struct TokenCacheMixinTraits {
			class CacheType : public TokenCache {
			public:
				CacheType() : TokenCache(CacheConfiguration())
				{}
			};

			using IdType = TokenId;
			using ValueType = state::TokenEntry;

			static uint8_t GetRawId(const IdType& id) {
				return static_cast<uint8_t>(id.unwrap());
			}

			static IdType GetId(const ValueType& entry) {
				return entry.tokenId();
			}

			static IdType MakeId(uint8_t id) {
				return IdType(id);
			}

			static ValueType CreateWithId(uint8_t id) {
				auto owner = Address{ { static_cast<uint8_t>(id * 2) }};
				auto definition = state::TokenDefinition(Height(), owner, 3, model::TokenProperties());
				return state::TokenEntry(MakeId(id), definition);
			}

			static ValueType CreateWithIdAndExpiration(uint8_t id, Height height) {
				// simulate behavior of lock info cache activation (so expiration is at specified height)
				auto properties = test::CreateTokenPropertiesWithDuration(BlockDuration(height.unwrap() - 1));
				auto definition = state::TokenDefinition(Height(1), Address(), 3, properties);
				return state::TokenEntry(MakeId(id), definition);
			}
		};

		struct TokenCacheDeltaModificationPolicy : public test:: DeltaInsertModificationPolicy {
			static void Modify(TokenCacheDelta& delta, const state::TokenEntry& entry) {
				auto tokenIter = delta.find(entry.tokenId());
				auto& entryFromCache = tokenIter.get();
				entryFromCache.increaseSupply(Amount(1));
			}
		};
	}

	DEFINE_CACHE_CONTAINS_TESTS(TokenCacheMixinTraits, ViewAccessor, _View)
	DEFINE_CACHE_CONTAINS_TESTS(TokenCacheMixinTraits, DeltaAccessor, _Delta)

	DEFINE_CACHE_ITERATION_TESTS(TokenCacheMixinTraits, ViewAccessor, _View)

	DEFINE_CACHE_ACCESSOR_TESTS(TokenCacheMixinTraits, ViewAccessor, MutableAccessor, _ViewMutable)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenCacheMixinTraits, ViewAccessor, ConstAccessor, _ViewConst)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenCacheMixinTraits, DeltaAccessor, MutableAccessor, _DeltaMutable)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenCacheMixinTraits, DeltaAccessor, ConstAccessor, _DeltaConst)

	DEFINE_CACHE_MUTATION_TESTS(TokenCacheMixinTraits, DeltaAccessor, _Delta)

	DEFINE_ACTIVE_PREDICATE_TESTS(TokenCacheMixinTraits, ViewAccessor, _View)
	DEFINE_ACTIVE_PREDICATE_TESTS(TokenCacheMixinTraits, DeltaAccessor, _Delta)

	DEFINE_CACHE_TOUCH_TESTS(TokenCacheMixinTraits, _Delta)

	DEFINE_DELTA_ELEMENTS_MIXIN_CUSTOM_TESTS(TokenCacheMixinTraits, TokenCacheDeltaModificationPolicy, _Delta)

	DEFINE_CACHE_BASIC_TESTS(TokenCacheMixinTraits,)

	// endregion

	// region remove

	namespace {
		constexpr size_t Default_Cache_Size = 10;

		void PopulateCache(LockedCacheDelta<TokenCacheDelta>& delta) {
			for (uint8_t i = 0; i < Default_Cache_Size; ++i)
				delta->insert(TokenCacheMixinTraits::CreateWithIdAndExpiration(static_cast<uint8_t>(i + 1), Height(i + 1)));
		}
	}

	TEST(TEST_CLASS, CanRemoveEternalToken) {
		// Arrange: populate cache with some entries and insert eternal token
		TokenCacheMixinTraits::CacheType cache;
		{
			auto delta = cache.createDelta();
			PopulateCache(delta);
			delta->insert(TokenCacheMixinTraits::CreateWithId(15));

			// Sanity:
			EXPECT_EQ(Default_Cache_Size + 1, delta->size());
			EXPECT_TRUE(delta->find(TokenId(15)).get().definition().isEternal());

			cache.commit();
		}

		{
			// Act: remove token with id 15
			auto delta = cache.createDelta();
			delta->remove(TokenId(15));
			cache.commit();
		}

		// Assert:
		EXPECT_FALSE(cache.createView()->contains(TokenId(15)));
	}

	TEST(TEST_CLASS, RemoveAndReinsertAddsNewEntryInHeightBasedMap) {
		// Arrange: populate cache with some entries and insert token with id 15 and expiry height 123
		TokenCacheMixinTraits::CacheType cache;
		{
			auto delta = cache.createDelta();
			PopulateCache(delta);
			delta->insert(TokenCacheMixinTraits::CreateWithIdAndExpiration(15, Height(123)));

			// Sanity:
			EXPECT_EQ(Default_Cache_Size + 1, delta->size());
			EXPECT_CONTAINS(delta->touch(Height(123)), TokenId(15));

			cache.commit();
		}

		{
			// Act: remove token with id 15
			auto delta = cache.createDelta();
			delta->remove(TokenId(15));
			cache.commit();
		}

		// Act: reinsert token with id 15 and expiry height 234
		auto delta = cache.createDelta();
		delta->insert(TokenCacheMixinTraits::CreateWithIdAndExpiration(15, Height(234)));

		// Assert: height based entry at height 234 was added
		auto deactivatingIds1 = delta->touch(Height(123));
		EXPECT_TRUE(deactivatingIds1.empty());

		auto deactivatingIds2 = delta->touch(Height(234));
		EXPECT_CONTAINS(deactivatingIds2, TokenId(15));
	}

	// endregion
}}
