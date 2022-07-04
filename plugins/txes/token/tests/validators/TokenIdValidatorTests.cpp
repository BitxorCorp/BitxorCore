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
#include "src/model/TokenIdGenerator.h"
#include "bitxorcore/utils/IntegerMath.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenIdValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenId,)

	TEST(TEST_CLASS, FailureWhenValidatingInvalidTokenId) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto pValidator = CreateTokenIdValidator();
		auto notification = model::TokenNonceNotification(owner, TokenNonce(), TokenId());

		// Act:
		auto result = test::ValidateNotification(*pValidator, notification);

		// Assert:
		EXPECT_EQ(Failure_Token_Invalid_Id, result);
	}

	// region nonce and id consistency

	namespace {
		auto CreateTokenNonceIdNotification(const Address& owner) {
			auto nonce = test::GenerateRandomValue<TokenNonce>();
			auto tokenId = model::GenerateTokenId(owner, nonce);
			return model::TokenNonceNotification(owner, nonce, tokenId);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingNotificationWithMatchingId) {
		// Arrange: note that CreateTokenNonceIdNotification creates proper token id
		auto owner = test::CreateRandomOwner();
		auto pValidator = CreateTokenIdValidator();
		auto notification = CreateTokenNonceIdNotification(owner);

		// Act:
		auto result = test::ValidateNotification(*pValidator, notification);

		// Assert:
		EXPECT_EQ(ValidationResult::Success, result);
	}

	TEST(TEST_CLASS, FailureWhenValidatingNotificationWithMismatchedId) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto pValidator = CreateTokenIdValidator();

		for (auto i = 0u; i < utils::GetNumBits<uint64_t>(); ++i) {
			// - note that CreateTokenNonceIdNotification creates proper token id
			auto notification = CreateTokenNonceIdNotification(owner);
			auto mutatedId = notification.TokenId.unwrap() ^ (1ull << i);
			notification.TokenId = TokenId(mutatedId);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification);

			// Assert:
			EXPECT_EQ(Failure_Token_Id_Mismatch, result) << i;
		}
	}

	// endregion
}}
