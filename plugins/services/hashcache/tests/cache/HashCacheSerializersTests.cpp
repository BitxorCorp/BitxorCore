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

#include "src/cache/HashCacheSerializers.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

#define TEST_CLASS HashCacheSerializersTests

	namespace {
		using Serializer = HashCachePrimarySerializer;

		static auto GenerateRandomTimestampedHash() {
			return state::TimestampedHash(test::GenerateRandomValue<Timestamp>(), test::GenerateRandomArray<Cached_Hash_Size>());
		}
	}

	TEST(TEST_CLASS, HashCachePrimarySerializer_SerializeValueReturnsEmptyString) {
		// Arrange:
		auto key = GenerateRandomTimestampedHash();

		// Act:
		auto result = Serializer::SerializeValue(key);

		// Assert:
		EXPECT_TRUE(result.empty());
	}

	TEST(TEST_CLASS, HashCachePrimarySerializer_KeyToBoundaryReturnsTimestamp) {
		// Arrange:
		auto key = GenerateRandomTimestampedHash();

		// Act:
		auto result = Serializer::KeyToBoundary(key);

		// Assert:
		EXPECT_EQ(result, key.Time.unwrap());
	}
}}
