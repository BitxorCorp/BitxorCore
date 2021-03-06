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

#include "bitxorcore/cache_core/BlockStatisticCacheSubCachePlugin.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/mocks/MockMemoryStream.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS BlockStatisticCacheSubCachePluginTests

	// region BlockStatisticCacheSummaryCacheStorage - saveAll / saveSummary

	namespace {
		void RunSaveConsistencyTest(size_t numValues) {
			// Arrange:
			auto bitxorcoreCache = test::CoreSystemCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			{
				auto cacheDelta = bitxorcoreCache.createDelta();
				auto& delta = cacheDelta.sub<BlockStatisticCache>();
				for (auto i = 100u; i < 100 + numValues; ++i)
					delta.insert(state::BlockStatistic(Height(i), Timestamp(i), Difficulty(i), BlockFeeMultiplier(i)));

				bitxorcoreCache.commit(Height(7));

				// Sanity:
				EXPECT_EQ(numValues, delta.size());
			}

			BlockStatisticCacheSubCachePlugin plugin(111);
			auto pStorage = plugin.createStorage();

			// Act: serialize via saveAll
			std::vector<uint8_t> bufferAll;
			mocks::MockMemoryStream streamAll(bufferAll);
			pStorage->saveAll(bitxorcoreCache.createView(), streamAll);

			// - serialize via saveSummary
			std::vector<uint8_t> bufferSummary;
			mocks::MockMemoryStream streamSummary(bufferSummary);
			pStorage->saveSummary(bitxorcoreCache.createDelta(), streamSummary);

			// Assert:
			EXPECT_EQ(bufferAll, bufferSummary);
		}
	}

	TEST(TEST_CLASS, SaveAllAndSaveSummaryWriteSameData_ZeroValues) {
		RunSaveConsistencyTest(0);
	}

	TEST(TEST_CLASS, SaveAllAndSaveSummaryWriteSameData_SingleValue) {
		RunSaveConsistencyTest(1);
	}

	TEST(TEST_CLASS, SaveAllAndSaveSummaryWriteSameData_MultipleValues) {
		RunSaveConsistencyTest(11);
	}

	// endregion

	// region BlockStatisticCacheSubCachePlugin

	TEST(TEST_CLASS, CanCreateCacheStorageViaPlugin) {
		// Arrange:
		BlockStatisticCacheSubCachePlugin plugin(111);

		// Act:
		auto pStorage = plugin.createStorage();

		// Assert:
		ASSERT_TRUE(!!pStorage);
		EXPECT_EQ("BlockStatisticCache", pStorage->name());
	}

	// endregion
}}
