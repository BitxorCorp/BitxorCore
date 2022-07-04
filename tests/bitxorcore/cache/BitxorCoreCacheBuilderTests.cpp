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

#include "bitxorcore/cache/BitxorCoreCacheBuilder.h"
#include "tests/test/cache/SimpleCache.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS BitxorCoreCacheBuilderTests

	namespace {
		template<size_t CacheId>
		class CustomSubCachePluginAdapter : public SubCachePluginAdapter<test::SimpleCacheT<CacheId>, test::SimpleCacheStorageTraits> {
		public:
			using SubCachePluginAdapter<test::SimpleCacheT<CacheId>, test::SimpleCacheStorageTraits>::SubCachePluginAdapter;
		};

		template<size_t CacheId>
		void AddSubCacheWithId(BitxorCoreCacheBuilder& builder) {
			builder.add<test::SimpleCacheStorageTraits>(std::make_unique<test::SimpleCacheT<CacheId>>());
		}

		template<size_t CacheId>
		void AddSubCachePluginWithId(BitxorCoreCacheBuilder& builder) {
			builder.add(std::make_unique<CustomSubCachePluginAdapter<CacheId>>(std::make_unique<test::SimpleCacheT<CacheId>>()));
		}

		size_t GetNumSubCaches(const BitxorCoreCache& cache) {
			return cache.storages().size();
		}
	}

	TEST(TEST_CLASS, CanCreateEmptyBitxorCoreCache) {
		// Arrange:
		BitxorCoreCacheBuilder builder;

		// Act:
		auto cache = builder.build();

		// Assert:
		EXPECT_EQ(0u, GetNumSubCaches(cache));
	}

	TEST(TEST_CLASS, CanCreateBitxorCoreCacheWithSingleSubCache_DefaultPlugin) {
		// Arrange:
		BitxorCoreCacheBuilder builder;
		AddSubCacheWithId<2>(builder);

		// Act:
		auto cache = builder.build();

		// Assert:
		EXPECT_EQ(1u, GetNumSubCaches(cache));
	}

	TEST(TEST_CLASS, CanCreateBitxorCoreCacheWithSingleSubCache_CustomPlugin) {
		// Arrange:
		BitxorCoreCacheBuilder builder;
		AddSubCachePluginWithId<2>(builder);

		// Act:
		auto cache = builder.build();

		// Assert:
		EXPECT_EQ(1u, GetNumSubCaches(cache));
	}

	TEST(TEST_CLASS, CanCreateBitxorCoreCacheWithMultipleSubCaches) {
		// Arrange:
		BitxorCoreCacheBuilder builder;
		AddSubCacheWithId<2>(builder);
		AddSubCachePluginWithId<6>(builder);
		AddSubCacheWithId<4>(builder);
		AddSubCachePluginWithId<8>(builder);

		// Act:
		auto cache = builder.build();

		// Assert:
		EXPECT_EQ(4u, GetNumSubCaches(cache));
	}

	TEST(TEST_CLASS, CannotAddMultipleSubCachesWithSameId) {
		// Arrange:
		BitxorCoreCacheBuilder builder;
		AddSubCacheWithId<2>(builder);
		AddSubCacheWithId<6>(builder);
		AddSubCacheWithId<4>(builder);

		// Act + Assert:
		EXPECT_THROW(AddSubCacheWithId<6>(builder), bitxorcore_invalid_argument);
		EXPECT_THROW(AddSubCachePluginWithId<6>(builder), bitxorcore_invalid_argument);
	}
}}
