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

#define TEST_CLASS TokenFlagsValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenFlags, Height())

	namespace {
		constexpr auto Revokable_Fork_Height = Height(1000);

		struct TokenFlagsTraits {
			using EnumType = model::TokenFlags;

			static constexpr auto Failure_Result = Failure_Token_Invalid_Flags;
			static constexpr auto CreateValidator = CreateTokenFlagsValidator;

			static std::vector<uint8_t> ValidValues() {
				return { 0x00, 0x02, 0x05, 0x07 }; // valid values prior to any fork
			}

			static std::vector<uint8_t> ValidValuesAfterRevokableFork() {
				return { 0x08, 0x09, 0x0F };
			}

			static std::vector<uint8_t> InvalidValuesAfterRevokableFork() {
				return { 0x10, 0x11, 0xFF };
			}

			static auto CreateNotification(model::TokenFlags value) {
				model::TokenProperties properties(value, 0, BlockDuration());
				return model::TokenPropertiesNotification(properties);
			}
		};

		void AssertValueValidationResult(ValidationResult expectedResult, model::TokenFlags value, Height height) {
			// Arrange:
			auto pValidator = TokenFlagsTraits::CreateValidator(Revokable_Fork_Height);
			auto notification = TokenFlagsTraits::CreateNotification(value);

			auto cache = test::CreateEmptyBitxorCoreCache();
			auto cacheView = cache.createView();
			auto readOnlyCache = cacheView.toReadOnly();
			auto validatorContext = test::CreateValidatorContext(height, readOnlyCache);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, validatorContext);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "value " << static_cast<uint64_t>(value) << " at " << height;
		}
	}

	// region basic

	TEST(TEST_CLASS, SuccessWhenProcessingValidValue) {
		for (auto value : TokenFlagsTraits::ValidValues())
			AssertValueValidationResult(ValidationResult::Success, static_cast<model::TokenFlags>(value), Height(1));
	}

	TEST(TEST_CLASS, FailureWhenProcessingInvalidValue) {
		for (auto value : TokenFlagsTraits::ValidValuesAfterRevokableFork())
			AssertValueValidationResult(TokenFlagsTraits::Failure_Result, static_cast<model::TokenFlags>(value), Height(1));

		for (auto value : TokenFlagsTraits::InvalidValuesAfterRevokableFork())
			AssertValueValidationResult(TokenFlagsTraits::Failure_Result, static_cast<model::TokenFlags>(value), Height(1));
	}

	// endregion

	// region revokable fork

	TEST(TEST_CLASS, FailureWhenRevokableFlagIsPresentBeforeRevokableFork) {
		for (auto value : TokenFlagsTraits::ValidValuesAfterRevokableFork()) {
			for (auto heightAdjustment : { Height(1), Height(100) }) {
				auto height = Revokable_Fork_Height - heightAdjustment;
				AssertValueValidationResult(TokenFlagsTraits::Failure_Result, static_cast<model::TokenFlags>(value), height);
			}
		}
	}

	TEST(TEST_CLASS, SuccessWhenRevokableFlagIsPresentAtRevokableFork) {
		for (auto value : TokenFlagsTraits::ValidValuesAfterRevokableFork())
			AssertValueValidationResult(ValidationResult::Success, static_cast<model::TokenFlags>(value), Revokable_Fork_Height);
	}

	TEST(TEST_CLASS, SuccessWhenRevokableFlagIsPresentAfterRevokableFork) {
		for (auto value : TokenFlagsTraits::ValidValuesAfterRevokableFork()) {
			for (auto heightAdjustment : { Height(1), Height(100) }) {
				auto height = Revokable_Fork_Height + heightAdjustment;
				AssertValueValidationResult(ValidationResult::Success, static_cast<model::TokenFlags>(value), height);
			}
		}
	}

	TEST(TEST_CLASS, FailureWhenProcessingInvalidValueAtRevokableFork) {
		for (auto value : TokenFlagsTraits::InvalidValuesAfterRevokableFork())
			AssertValueValidationResult(TokenFlagsTraits::Failure_Result, static_cast<model::TokenFlags>(value), Revokable_Fork_Height);
	}

	// endregion
}}
