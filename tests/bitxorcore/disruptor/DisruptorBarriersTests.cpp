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

#include "bitxorcore/disruptor/DisruptorBarriers.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace disruptor {

#define TEST_CLASS DisruptorBarriersTests

	TEST(TEST_CLASS, CanCreateBarriersWithoutBarriers) {
		// Arrange+Act:
		DisruptorBarriers barriers(0);

		// Assert:
		EXPECT_EQ(0u, barriers.size());
	}

	TEST(TEST_CLASS, CanCreateBarriersWithASingleBarrier) {
		// Arrange+Act:
		DisruptorBarriers barriers(1);

		// Assert:
		EXPECT_EQ(1u, barriers.size());
		EXPECT_EQ(0u, barriers[0].position());
		EXPECT_EQ(0u, barriers[0].level());
	}

	TEST(TEST_CLASS, CanCreateBarriersWithMultipleBarriers) {
		// Arrange+Act:
		DisruptorBarriers barriers(10);

		// Assert:
		EXPECT_EQ(10u, barriers.size());
		for (auto i = 0u; i < 10; ++i) {
			EXPECT_EQ(0u, barriers[i].position());
			EXPECT_EQ(i, barriers[i].level());
		}
	}

	TEST(TEST_CLASS, CanAccessBarriersViaConstInterface) {
		// Arrange+Act:
		DisruptorBarriers testedBarriers(10);

		// Assert:
		const auto& barriers = testedBarriers;
		EXPECT_EQ(10u, barriers.size());
		for (auto i = 0u; i < 10; ++i) {
			EXPECT_EQ(0u, barriers[i].position());
			EXPECT_EQ(i, barriers[i].level());
		}
	}
}}
