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

#include "bitxorcore/cache/CacheHeight.h"
#include "tests/test/nodeps/LockTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS CacheHeightTests

	TEST(TEST_CLASS, HeightIsInitiallyZero) {
		// Act:
		CacheHeight height;

		// Assert:
		EXPECT_EQ(Height(0), height.view().get());
	}

	TEST(TEST_CLASS, CanChangeHeight) {
		// Arrange:
		CacheHeight height;

		// Act:
		height.modifier().set(Height(343));

		// Assert:
		EXPECT_EQ(Height(343), height.view().get());
	}

	// region synchronization

	namespace {
		auto CreateLockProvider() {
			return std::make_unique<CacheHeight>();
		}
	}

	DEFINE_LOCK_PROVIDER_TESTS(TEST_CLASS)

	// endregion
}}
