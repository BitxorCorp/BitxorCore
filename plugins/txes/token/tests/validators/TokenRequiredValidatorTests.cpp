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
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/core/ResolverTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRequiredValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(RequiredToken,)

	namespace {
		constexpr auto Token_Expiry_Height = Height(150);

		struct ResolvedTokenTraits {
			static constexpr auto Default_Id = TokenId(110);
		};

		struct UnresolvedTokenTraits {
			// custom resolver doubles unresolved token ids
			static constexpr auto Default_Id = UnresolvedTokenId(110 ^ 0xFFFF'FFFF'FFFF'FFFF);
		};
	}

#define TOKEN_ID_TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Resolved) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ResolvedTokenTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Unresolved) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<UnresolvedTokenTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// region zero property mask

	namespace {
		template<typename TTokenId>
		void AssertValidationResult(
				ValidationResult expectedResult,
				TTokenId affectedTokenId,
				Height height,
				const model::ResolvableAddress& notificationOwner,
				const Address& artifactOwner) {
			// Arrange:
			auto pValidator = CreateRequiredTokenValidator();

			// - create the notification
			model::TokenRequiredNotification notification(notificationOwner, affectedTokenId);

			// - create the validator context
			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			auto delta = cache.createDelta();
			test::AddToken(delta, ResolvedTokenTraits::Default_Id, Height(50), BlockDuration(100), artifactOwner);

			auto readOnlyCache = delta.toReadOnly();
			auto context = test::CreateValidatorContext(height, readOnlyCache);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, context);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "height " << height << ", id " << affectedTokenId;
		}

		template<typename TTokenId>
		void AssertValidationResult(ValidationResult expectedResult, TTokenId affectedTokenId, Height height) {
			auto owner = test::CreateRandomOwner();
			AssertValidationResult(expectedResult, affectedTokenId, height, owner, owner);
		}
	}

	TOKEN_ID_TRAITS_BASED_TEST(FailureWhenTokenIsUnknown) {
		auto unknownTokenId = TTraits::Default_Id + decltype(TTraits::Default_Id)(1);
		AssertValidationResult(Failure_Token_Expired, unknownTokenId, Height(100));
	}

	TOKEN_ID_TRAITS_BASED_TEST(FailureWhenTokenExpired) {
		AssertValidationResult(Failure_Token_Expired, TTraits::Default_Id, Token_Expiry_Height);
	}

	TOKEN_ID_TRAITS_BASED_TEST(FailureWhenTokenOwnerDoesNotMatch) {
		auto owner1 = test::CreateRandomOwner();
		auto owner2 = test::CreateRandomOwner();
		AssertValidationResult(Failure_Token_Owner_Conflict, TTraits::Default_Id, Height(100), owner1, owner2);
	}

	TOKEN_ID_TRAITS_BASED_TEST(SuccessWhenTokenIsActiveAndOwnerMatches) {
		AssertValidationResult(ValidationResult::Success, TTraits::Default_Id, Height(100));
	}

	TOKEN_ID_TRAITS_BASED_TEST(SuccessWhenTokenIsActiveAndOwnerMatches_UnresolvedAddress) {
		auto owner = test::CreateRandomOwner();
		AssertValidationResult(ValidationResult::Success, TTraits::Default_Id, Height(100), test::UnresolveXor(owner), owner);
	}

	// endregion

	// region nonzero property mask

	namespace {
		template<typename TTokenId>
		void AssertPropertyFlagMaskValidationResult(
				ValidationResult expectedResult,
				TTokenId affectedTokenId,
				uint8_t notificationPropertyFlagMask,
				uint8_t tokenPropertyFlagMask) {
			// Arrange:
			auto pValidator = CreateRequiredTokenValidator();

			// - create the notification
			auto owner = test::CreateRandomOwner();
			model::TokenRequiredNotification notification(owner, affectedTokenId, notificationPropertyFlagMask);

			// - create the validator context
			auto height = Height(50);
			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			auto delta = cache.createDelta();

			{
				// need to set custom property flags, so can't use regular helpers (e.g. test::AddToken)
				auto& tokenCacheDelta = delta.sub<cache::TokenCache>();

				model::TokenProperties properties(static_cast<model::TokenFlags>(tokenPropertyFlagMask), 0, BlockDuration(100));
				auto definition = state::TokenDefinition(height, owner, 1, properties);
				tokenCacheDelta.insert(state::TokenEntry(ResolvedTokenTraits::Default_Id, definition));
			}

			auto readOnlyCache = delta.toReadOnly();
			auto context = test::CreateValidatorContext(height, readOnlyCache);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, context);

			// Assert:
			EXPECT_EQ(expectedResult, result)
					<< "notificationPropertyFlagMask " << static_cast<uint16_t>(notificationPropertyFlagMask)
					<< "tokenPropertyFlagMask " << static_cast<uint16_t>(tokenPropertyFlagMask);
		}
	}

	TOKEN_ID_TRAITS_BASED_TEST(FailureWhenPropertyFlagMaskDoesNotOverlap) {
		// Assert: 101, 010
		AssertPropertyFlagMaskValidationResult(Failure_Token_Required_Property_Flag_Unset, TTraits::Default_Id, 0x05, 0x02);
	}

	TOKEN_ID_TRAITS_BASED_TEST(FailureWhenPropertyFlagMaskPartiallyOverlaps) {
		// Assert: 101, 110
		AssertPropertyFlagMaskValidationResult(Failure_Token_Required_Property_Flag_Unset, TTraits::Default_Id, 0x05, 0x06);
	}

	TOKEN_ID_TRAITS_BASED_TEST(SuccessWhenPropertyFlagMaskIsExactMatch) {
		// Assert: 101, 101
		AssertPropertyFlagMaskValidationResult(ValidationResult::Success, TTraits::Default_Id, 0x05, 0x05);
	}

	TOKEN_ID_TRAITS_BASED_TEST(SuccessWhenPropertyFlagMaskIsSubset) {
		// Assert: 101, 111
		AssertPropertyFlagMaskValidationResult(ValidationResult::Success, TTraits::Default_Id, 0x05, 0x07);
	}

	// endregion
}}
