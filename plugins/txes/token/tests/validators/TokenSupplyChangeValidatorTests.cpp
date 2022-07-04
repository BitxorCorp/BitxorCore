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

#include "src/validators/Validators.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenSupplyChangeValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenSupplyChange,)

	namespace {
		void AssertValidationResult(ValidationResult expectedResult, model::TokenSupplyChangeAction action, Amount delta) {
			// Arrange:
			model::TokenSupplyChangeNotification notification(Address(), UnresolvedTokenId(), action, delta);
			auto pValidator = CreateTokenSupplyChangeValidator();

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "action " << utils::to_underlying_type(action) << ", delta " << delta;
		}
	}

	// region action

	namespace {
		constexpr auto ToAction(int32_t action) {
			return static_cast<model::TokenSupplyChangeAction>(action);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingValidAction) {
		for (auto action : { 0x00, 0x01 })
			AssertValidationResult(ValidationResult::Success, ToAction(action), Amount(123));
	}

	TEST(TEST_CLASS, FailureWhenValidatingInvalidAction) {
		for (auto action : { 0x02, 0xFF })
			AssertValidationResult(Failure_Token_Invalid_Supply_Change_Action, ToAction(action), Amount(123));
	}

	// endregion

	// region amount

	TEST(TEST_CLASS, SuccessWhenDeltaIsNonzero) {
		for (auto action : { 0x00, 0x01 })
			AssertValidationResult(ValidationResult::Success, ToAction(action), Amount(1));
	}

	TEST(TEST_CLASS, FailureWhenDeltaIsZero) {
		for (auto action : { 0x00, 0x01 })
			AssertValidationResult(Failure_Token_Invalid_Supply_Change_Amount, ToAction(action), Amount());
	}

	// endregion
}}
