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

#include "bitxorcore/cache/SummaryAwareSubCachePluginAdapter.h"
#include "tests/test/cache/SimpleCache.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS SummaryAwareSubCachePluginAdapterTests

	namespace {
		class SimpleCacheSummaryCacheStorage : public SummaryCacheStorage<test::SimpleCache> {
		public:
			using SummaryCacheStorage<test::SimpleCache>::SummaryCacheStorage;

		public:
			void saveAll(const BitxorCoreCacheView&, io::OutputStream&) const override {
				// do nothing
			}

			void saveSummary(const BitxorCoreCacheDelta&, io::OutputStream&) const override {
				// do nothing
			}

			void loadAll(io::InputStream&, size_t) override {
				// do nothing
			}
		};

		void AssertCanCreateStorageViaPlugin(test::SimpleCacheViewMode mode, const std::string& expectedStorageName) {
			// Arrange:
			using PluginType = SummaryAwareSubCachePluginAdapter<
				test::SimpleCacheT<3>,
				test::SimpleCacheStorageTraits,
				SimpleCacheSummaryCacheStorage>;
			PluginType plugin(std::make_unique<test::SimpleCacheT<3>>(mode));

			// Act:
			auto pStorage = plugin.createStorage();

			// Assert:
			ASSERT_TRUE(!!pStorage);
			EXPECT_EQ(expectedStorageName, pStorage->name());
		}
	}

	TEST(TEST_CLASS, CanCreateCacheStorageViaPluginForFullStorage) {
		AssertCanCreateStorageViaPlugin(test::SimpleCacheViewMode::Iterable, "SimpleCache");
	}

	TEST(TEST_CLASS, CanCreateCacheStorageViaPluginForSummaryStorage) {
		AssertCanCreateStorageViaPlugin(test::SimpleCacheViewMode::Basic, "SimpleCache_summary");
	}
}}
