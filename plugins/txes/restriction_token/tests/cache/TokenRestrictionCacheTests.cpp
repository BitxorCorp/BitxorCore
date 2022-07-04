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

#include "src/cache/TokenRestrictionCache.h"
#include "tests/test/TokenRestrictionTestUtils.h"
#include "tests/test/cache/CacheBasicTests.h"
#include "tests/test/cache/CacheMixinsTests.h"
#include "tests/test/cache/DeltaElementsMixinTests.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS TokenRestrictionCacheTests

	// region mixin traits based tests

	namespace {
		struct TokenRestrictionCacheMixinTraits {
			class CacheType : public TokenRestrictionCache {
			public:
				CacheType() : TokenRestrictionCache(CacheConfiguration(), model::NetworkIdentifier::Zero)
				{}
			};

			using IdType = Hash256;
			using ValueType = state::TokenRestrictionEntry;

			static uint8_t GetRawId(const IdType& id) {
				return id[0];
			}

			static IdType GetId(const ValueType& entry) {
				return entry.uniqueKey();
			}

			static IdType MakeId(uint8_t id) {
				return IdType{ { id } };
			}

			static ValueType CreateWithId(uint8_t id) {
				return state::TokenRestrictionEntry(test::GenerateTokenRestrictionEntry(MakeId(id)));
			}
		};

		struct TokenRestrictionCacheDeltaModificationPolicy : public test:: DeltaInsertModificationPolicy {
			static void Modify(TokenRestrictionCacheDelta& delta, const state::TokenRestrictionEntry& entry) {
				auto& entryFromCache = delta.find(entry.uniqueKey()).get();
				auto& restriction = entryFromCache.asAddressRestriction();
				restriction.set(1, 1);
			}
		};
	}

	DEFINE_CACHE_CONTAINS_TESTS(TokenRestrictionCacheMixinTraits, ViewAccessor, _View)
	DEFINE_CACHE_CONTAINS_TESTS(TokenRestrictionCacheMixinTraits, DeltaAccessor, _Delta)

	DEFINE_CACHE_ITERATION_TESTS(TokenRestrictionCacheMixinTraits, ViewAccessor, _View)

	DEFINE_CACHE_ACCESSOR_TESTS(TokenRestrictionCacheMixinTraits, ViewAccessor, MutableAccessor, _ViewMutable)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenRestrictionCacheMixinTraits, ViewAccessor, ConstAccessor, _ViewConst)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenRestrictionCacheMixinTraits, DeltaAccessor, MutableAccessor, _DeltaMutable)
	DEFINE_CACHE_ACCESSOR_TESTS(TokenRestrictionCacheMixinTraits, DeltaAccessor, ConstAccessor, _DeltaConst)

	DEFINE_CACHE_MUTATION_TESTS(TokenRestrictionCacheMixinTraits, DeltaAccessor, _Delta)

	DEFINE_DELTA_ELEMENTS_MIXIN_CUSTOM_TESTS(TokenRestrictionCacheMixinTraits, TokenRestrictionCacheDeltaModificationPolicy, _Delta)

	DEFINE_CACHE_BASIC_TESTS(TokenRestrictionCacheMixinTraits,)

	// endregion

	// region custom tests

	TEST(TEST_CLASS, CacheWrappersExposeNetworkIdentifier) {
		// Arrange:
		auto networkIdentifier = static_cast<model::NetworkIdentifier>(18);
		TokenRestrictionCache cache(CacheConfiguration(), networkIdentifier);

		// Act + Assert:
		EXPECT_EQ(networkIdentifier, cache.createView()->networkIdentifier());
		EXPECT_EQ(networkIdentifier, cache.createDelta()->networkIdentifier());
		EXPECT_EQ(networkIdentifier, cache.createDetachedDelta().tryLock()->networkIdentifier());
	}

	// endregion
}}
