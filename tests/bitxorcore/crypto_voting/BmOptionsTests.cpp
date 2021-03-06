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

#include "bitxorcore/crypto_voting/BmOptions.h"
#include "tests/test/nodeps/Alignment.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace crypto {

#define TEST_CLASS BmOptionsTests

#define OPTIONS_FIELDS FIELD(StartKeyIdentifier) FIELD(EndKeyIdentifier)

	TEST(TEST_CLASS, OptionsHasExpectedSize) {
		// Arrange:
		auto expectedSize = 0u;

#define FIELD(X) expectedSize += SizeOf32<decltype(BmOptions::X)>();
		OPTIONS_FIELDS
#undef FIELD

		// Assert:
		EXPECT_EQ(expectedSize, sizeof(BmOptions));
		EXPECT_EQ(16u, sizeof(BmOptions));
	}

	TEST(TEST_CLASS, OptionsHasProperAlignment) {
#define FIELD(X) EXPECT_ALIGNED(BmOptions, X);
		OPTIONS_FIELDS
#undef FIELD

		EXPECT_EQ(0u, sizeof(BmOptions) % 8);
	}

#undef OPTIONS_FIELDS
}}
