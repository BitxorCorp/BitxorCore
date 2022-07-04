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

#include "bitxorcore/cache/RelockableDetachedBitxorCoreCache.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS RelockableDetachedBitxorCoreCacheTests

	namespace {
		void SetHeight(BitxorCoreCache& cache, Height height) {
			auto delta = cache.createDelta();
			cache.commit(height);
		}
	}

	TEST(TEST_CLASS, CanCreateRelockableDetachedBitxorCoreCache) {
		// Arrange:
		auto cache = test::CreateEmptyBitxorCoreCache();
		SetHeight(cache, Height(7));

		// Act:
		RelockableDetachedBitxorCoreCache detachedBitxorCoreCache(cache);

		// Assert: the detached cache should have the correct height and be lockable
		EXPECT_EQ(Height(7), detachedBitxorCoreCache.height());
		EXPECT_TRUE(!!detachedBitxorCoreCache.getAndTryLock());
	}

	TEST(TEST_CLASS, RelockableDetachedBitxorCoreCacheIsInvalidatedWhenUnderlyingCacheChanges) {
		// Arrange:
		auto cache = test::CreateEmptyBitxorCoreCache();
		SetHeight(cache, Height(7));

		// - create the detached cache
		RelockableDetachedBitxorCoreCache detachedBitxorCoreCache(cache);

		// Act: invalidate it
		SetHeight(cache, Height(11));

		// Assert: the detached cache should have the original height and not be lockable
		EXPECT_EQ(Height(7), detachedBitxorCoreCache.height());
		EXPECT_FALSE(!!detachedBitxorCoreCache.getAndTryLock());
	}

	TEST(TEST_CLASS, RelockableDetachedBitxorCoreCacheCanBeRebasedOnTopOfUnderlyingCache) {
		// Arrange:
		auto cache = test::CreateEmptyBitxorCoreCache();
		SetHeight(cache, Height(7));

		// - create the detached cache
		RelockableDetachedBitxorCoreCache detachedBitxorCoreCache(cache);

		// - invalidate it
		SetHeight(cache, Height(11));

		// Act: rebase it
		{
			auto pDelta = detachedBitxorCoreCache.rebaseAndLock();

			// Assert: the returned delta should always be a valid pointer
			EXPECT_TRUE(!!pDelta);
		}

		// Assert: the detached cache should have the new height and be lockable
		EXPECT_EQ(Height(11), detachedBitxorCoreCache.height());
		EXPECT_TRUE(!!detachedBitxorCoreCache.getAndTryLock());
	}
}}
