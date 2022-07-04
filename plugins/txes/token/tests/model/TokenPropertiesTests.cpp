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

#include "src/model/TokenProperties.h"
#include "bitxorcore/utils/Casting.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/nodeps/Equality.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS TokenPropertiesTests

	// region ctor

	TEST(TEST_CLASS, CanCreateDefaultTokenProperties) {
		// Act:
		TokenProperties properties;

		// Assert:
		EXPECT_EQ(TokenFlags::None, properties.flags());
		EXPECT_EQ(0u, properties.divisibility());
		EXPECT_EQ(BlockDuration(), properties.duration());

		for (auto flag = 1u; flag < utils::to_underlying_type(TokenFlags::All); flag <<= 1)
			EXPECT_FALSE(properties.is(static_cast<TokenFlags>(flag))) << "flag " << flag;
	}

	TEST(TEST_CLASS, CanCreateTokenPropertiesAroundValues) {
		// Act:
		TokenProperties properties(TokenFlags::Supply_Mutable | TokenFlags::Restrictable, 5, BlockDuration(234));

		// Assert:
		EXPECT_EQ(TokenFlags::Supply_Mutable | TokenFlags::Restrictable, properties.flags());
		EXPECT_EQ(5u, properties.divisibility());
		EXPECT_EQ(BlockDuration(234), properties.duration());

		for (auto flag = 1u; flag < utils::to_underlying_type(TokenFlags::All); flag <<= 1)
			EXPECT_EQ(!!(flag & 0x05), properties.is(static_cast<TokenFlags>(flag))) << "flag " << flag;
	}

	// endregion

	// region equality operators

	namespace {
		std::unordered_set<std::string> GetEqualTags() {
			return { "default", "copy" };
		}

		std::unordered_map<std::string, TokenProperties> GenerateEqualityInstanceMap() {
			return {
				{ "default", test::CreateTokenPropertiesFromValues(2, 7, 5) },
				{ "copy", test::CreateTokenPropertiesFromValues(2, 7, 5) },

				{ "diff[0]", test::CreateTokenPropertiesFromValues(1, 7, 5) },
				{ "diff[1]", test::CreateTokenPropertiesFromValues(2, 9, 5) },
				{ "diff[2]", test::CreateTokenPropertiesFromValues(2, 7, 6) },
				{ "reverse", test::CreateTokenPropertiesFromValues(5, 7, 2) },
				{ "diff-all", test::CreateTokenPropertiesFromValues(1, 8, 6) }
			};
		}
	}

	TEST(TEST_CLASS, OperatorEqualReturnsTrueOnlyForEqualValues) {
		test::AssertOperatorEqualReturnsTrueForEqualObjects("default", GenerateEqualityInstanceMap(), GetEqualTags());
	}

	TEST(TEST_CLASS, OperatorNotEqualReturnsTrueOnlyForUnequalValues) {
		test::AssertOperatorNotEqualReturnsTrueForUnequalObjects("default", GenerateEqualityInstanceMap(), GetEqualTags());
	}

	// endregion
}}
