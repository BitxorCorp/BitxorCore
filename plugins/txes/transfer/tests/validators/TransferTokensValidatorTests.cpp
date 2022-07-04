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

#define TEST_CLASS TransferTokensValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TransferTokens,)

	namespace {
		constexpr auto Success_Result = ValidationResult::Success;

		void AssertValidationResult(ValidationResult expectedResult, const std::vector<model::UnresolvedToken>& tokens) {
			// Arrange:
			model::TransferTokensNotification notification(static_cast<uint8_t>(tokens.size()), tokens.data());
			auto pValidator = CreateTransferTokensValidator();

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingNotificationWithZeroTokens) {
		AssertValidationResult(Success_Result, {});
	}

	TEST(TEST_CLASS, SuccessWhenValidatingNotificationWithOneToken) {
		AssertValidationResult(Success_Result, { { UnresolvedTokenId(71), Amount(5) } });
	}

	TEST(TEST_CLASS, SuccessWhenValidatingNotificationWithMultipleOrderedTokens) {
		AssertValidationResult(
				Success_Result,
				{ { UnresolvedTokenId(71), Amount(5) }, { UnresolvedTokenId(182), Amount(4) }, { UnresolvedTokenId(200), Amount(1) } });
	}

	TEST(TEST_CLASS, FailureWhenValidatingNotificationWithMultipleOutOfOrderTokens) {
		// Assert: first and second are out of order
		AssertValidationResult(
				Failure_Transfer_Out_Of_Order_Tokens,
				{ { UnresolvedTokenId(200), Amount(5) }, { UnresolvedTokenId(71), Amount(1) }, { UnresolvedTokenId(182), Amount(4) } });

		// - second and third are out of order
		AssertValidationResult(
				Failure_Transfer_Out_Of_Order_Tokens,
				{ { UnresolvedTokenId(71), Amount(5) }, { UnresolvedTokenId(200), Amount(1) }, { UnresolvedTokenId(182), Amount(4) } });
	}

	TEST(TEST_CLASS, FailureWhenValidatingNotificationWithMultipleTransfersOfSameToken) {
		// Assert: create a transaction with multiple (in order) transfers for the same token
		AssertValidationResult(
				Failure_Transfer_Out_Of_Order_Tokens,
				{
					{ UnresolvedTokenId(71), Amount(5) },
					{ UnresolvedTokenId(182), Amount(4) },
					{ UnresolvedTokenId(182), Amount(4) },
					{ UnresolvedTokenId(200), Amount(1) }
				});
	}
}}
