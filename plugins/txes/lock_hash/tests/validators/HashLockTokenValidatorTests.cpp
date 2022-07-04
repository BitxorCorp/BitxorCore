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
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS HashLockTokenValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(HashLockToken, TokenId(11), Amount(123))

	namespace {
		constexpr TokenId Currency_Token_Id(1234);

		void AssertValidationResult(ValidationResult expectedResult, TokenId tokenId, Amount bondedAmount, Amount requiredBondedAmount) {
			// Arrange:
			auto pValidator = CreateHashLockTokenValidator(Currency_Token_Id, requiredBondedAmount);
			auto notification = model::HashLockTokenNotification({ test::UnresolveXor(tokenId), bondedAmount });

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, test::CreateEmptyBitxorCoreCache());

			// Assert:
			EXPECT_EQ(expectedResult, result) << "notification with id " << tokenId << " and amount " << bondedAmount;
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingNotificationWithProperTokenIdAndAmount) {
		AssertValidationResult(ValidationResult::Success, Currency_Token_Id, Amount(500), Amount(500));
	}

	TEST(TEST_CLASS, FailureWhenValidatingNotificationWithInvalidTokenId) {
		auto tokenId = test::GenerateRandomValue<TokenId>();
		AssertValidationResult(Failure_LockHash_Invalid_Token_Id, tokenId, Amount(500), Amount(500));
	}

	TEST(TEST_CLASS, FailureWhenValidatingNotificationWithInvalidAmount) {
		for (auto amount : { 0ull, 1ull, 10ull, 100ull, 499ull, 501ull, 1000ull })
			AssertValidationResult(Failure_LockHash_Invalid_Token_Amount, TokenId(123), Amount(amount), Amount(500));
	}
}}
