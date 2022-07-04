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

#include "bitxorcore/io/StringOutputStream.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace io {

#define TEST_CLASS StringOutputStreamTests

	TEST(TEST_CLASS, WriteStoresDataInUnderlyingBuffer) {
		// Act:
		StringOutputStream output(25);
		auto buffer = test::GenerateRandomArray<25>();

		// Act:
		output.write(buffer);

		// Assert:
		std::string expected;
		for (auto byte : buffer)
			expected.push_back(static_cast<char>(byte));

		EXPECT_EQ(25u, output.str().size());
		EXPECT_EQ(expected, output.str());
	}

	TEST(TEST_CLASS, FlushIsNoOp) {
		// Arrange:
		StringOutputStream output(0);

		// Act:
		output.flush();

		// Assert:
		EXPECT_EQ(0u, output.str().size());
	}

	TEST(TEST_CLASS, FlushDoesNotAffectWrite) {
		// Act:
		StringOutputStream output(25);
		output.write(test::GenerateRandomArray<25>());

		// Sanity:
		EXPECT_EQ(25u, output.str().size());

		// Act:
		output.flush();

		// Assert:
		EXPECT_EQ(25u, output.str().size());
	}
}}
