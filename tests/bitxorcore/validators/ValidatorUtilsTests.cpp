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

#include "bitxorcore/validators/ValidatorUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS ValidatorUtilsTests

	TEST(TEST_CLASS, ValidateLessThanOrEqualReturnsCorrectValue) {
		// Arrange:
		constexpr auto Failure_Result = static_cast<ValidationResult>(12345);

		// Act + Assert:
		EXPECT_EQ(ValidationResult::Success, ValidateLessThanOrEqual(0, 101, Failure_Result));
		EXPECT_EQ(ValidationResult::Success, ValidateLessThanOrEqual(100, 101, Failure_Result));
		EXPECT_EQ(ValidationResult::Success, ValidateLessThanOrEqual(101, 101, Failure_Result));

		EXPECT_EQ(Failure_Result, ValidateLessThanOrEqual(102, 101, Failure_Result));
		EXPECT_EQ(Failure_Result, ValidateLessThanOrEqual(999, 101, Failure_Result));
	}
}}
