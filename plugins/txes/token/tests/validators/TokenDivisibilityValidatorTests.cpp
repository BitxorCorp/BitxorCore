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
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenDivisibilityValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenDivisibility, 0)

	namespace {
		void AddToken(cache::BitxorCoreCache& cache, const Address& owner, TokenId tokenId, uint8_t divisibility) {
			auto properties = model::TokenProperties(model::TokenFlags::None, divisibility, BlockDuration());
			auto definition = state::TokenDefinition(Height(50), owner, 3, properties);
			auto tokenEntry = state::TokenEntry(tokenId, definition);

			auto delta = cache.createDelta();
			delta.sub<cache::TokenCache>().insert(tokenEntry);
			cache.commit(Height());
		}

		void AssertDivisibilityValidationResult(
				ValidationResult expectedResult,
				uint8_t initialDivisibility,
				uint8_t notificationDivisibility,
				uint8_t maxDivisibility) {
			// Arrange:
			auto pValidator = CreateTokenDivisibilityValidator(maxDivisibility);

			auto owner = test::CreateRandomOwner();
			auto tokenId = TokenId(123);
			auto properties = model::TokenProperties(model::TokenFlags::None, notificationDivisibility, BlockDuration());
			auto notification = model::TokenDefinitionNotification(owner, tokenId, properties);

			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			if (0 < initialDivisibility)
				AddToken(cache, owner, tokenId, initialDivisibility);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result)
					<< "initial " << static_cast<uint32_t>(initialDivisibility)
					<< ", notification " << static_cast<uint32_t>(notificationDivisibility)
					<< ", max " << static_cast<uint32_t>(maxDivisibility);
		}

		void AssertDivisibilityValidationResultRange(const consumer<uint8_t>& assertStep) {
			for (auto divisibility : std::initializer_list<uint8_t>{ 0, 5, 9 })
				assertStep(divisibility);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingDivisibilityLessThanMax_New) {
		AssertDivisibilityValidationResultRange([](auto divisibility) {
			AssertDivisibilityValidationResult(ValidationResult::Success, 0, divisibility, 10);
		});
	}

	TEST(TEST_CLASS, SuccessWhenValidatingDivisibilityLessThanMax_Existing) {
		AssertDivisibilityValidationResultRange([](auto divisibility) {
			AssertDivisibilityValidationResult(ValidationResult::Success, 3, 3 ^ divisibility, 10);
		});

		// following is valid even though notification divisibility is greater than max divisibility because 10 ^ 11 == 1 < 10
		AssertDivisibilityValidationResult(ValidationResult::Success, 10, 11, 10);
	}

	TEST(TEST_CLASS, SuccessWhenValidatingDivisibilityEqualToMax_New) {
		AssertDivisibilityValidationResult(ValidationResult::Success, 0, 10, 10);
	}

	TEST(TEST_CLASS, SuccessWhenValidatingDivisibilityEqualToMax_Existing) {
		AssertDivisibilityValidationResultRange([](auto divisibility) {
			AssertDivisibilityValidationResult(ValidationResult::Success, static_cast<uint8_t>(10 ^ divisibility), divisibility, 10);
		});
	}

	TEST(TEST_CLASS, FailureWhenValidatingDivisibilityGreaterThanMax_New) {
		AssertDivisibilityValidationResultRange([](auto divisibility) {
			AssertDivisibilityValidationResult(Failure_Token_Invalid_Divisibility, 0, static_cast<uint8_t>(11 + divisibility), 10);
		});
	}

	TEST(TEST_CLASS, FailureWhenValidatingDivisibilityGreaterThanMax_Existing) {
		AssertDivisibilityValidationResultRange([](auto divisibility) {
			AssertDivisibilityValidationResult(Failure_Token_Invalid_Divisibility, 3, static_cast<uint8_t>(3 ^ (11 + divisibility)), 10);
		});
	}
}}
