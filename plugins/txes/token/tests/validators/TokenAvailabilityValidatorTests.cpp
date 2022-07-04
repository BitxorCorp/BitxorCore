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
#include "src/cache/TokenCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/constants.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenAvailabilityValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenAvailability,)

	namespace {
		model::TokenDefinitionNotification CreateNotification(
				const Address& owner,
				TokenId id,
				const model::TokenProperties& properties) {
			return model::TokenDefinitionNotification(owner, id, properties);
		}

		void AssertValidationResult(
				ValidationResult expectedResult,
				const cache::BitxorCoreCache& cache,
				Height height,
				const model::TokenDefinitionNotification& notification) {
			// Arrange:
			auto pValidator = CreateTokenAvailabilityValidator();

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache, height);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "height " << height << ", id " << notification.TokenId;
		}

		void AddToken(cache::BitxorCoreCache& cache, TokenId id, Amount tokenSupply, const Address& owner, Amount ownerSupply) {
			auto delta = cache.createDelta();
			test::AddToken(delta, id, Height(50), BlockDuration(100), tokenSupply, owner);
			test::AddTokenOwner(delta, id, owner, ownerSupply);
			cache.commit(Height());
		}

		void AddEternalToken(cache::BitxorCoreCache& cache, TokenId id, const Address& owner) {
			auto delta = cache.createDelta();
			test::AddEternalToken(delta, id, Height(50), owner);
			test::AddTokenOwner(delta, id, owner, Amount());
			cache.commit(Height());
		}
	}

	// region unknown / inactive token

	TEST(TEST_CLASS, SuccessWhenTokenIsUnknown) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, 0);
		auto notification = CreateNotification(owner, TokenId(123), properties);

		// - seed the cache with an unrelated token
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, TokenId(100), Amount(500), owner, Amount(400));

		// Assert:
		AssertValidationResult(ValidationResult::Success, cache, Height(100), notification);
	}

	TEST(TEST_CLASS, FailureWhenTokenExistsButIsNotActive) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 3, 200);
		auto notification = CreateNotification(owner, TokenId(123), properties);

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, TokenId(123), Amount(0), owner, Amount(0));

		// Assert: token expires at height 150
		AssertValidationResult(Failure_Token_Expired, cache, Height(150), notification);
		AssertValidationResult(Failure_Token_Expired, cache, Height(151), notification);
		AssertValidationResult(Failure_Token_Expired, cache, Height(999), notification);
	}

	TEST(TEST_CLASS, FailureWhenNotificationOwnerIsNotTokenOwner) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 3, 200);
		auto notification = CreateNotification(owner, TokenId(123), properties);

		// - seed the cache with an active token with the same id and zero supply
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, TokenId(123), Amount(0), test::CreateRandomOwner(), Amount(0));

		// Assert:
		AssertValidationResult(Failure_Token_Owner_Conflict, cache, Height(100), notification);
	}

	// endregion

	// region properties check

	namespace {
		void AssertEternalPropertiesCheck(ValidationResult expectedResult, const model::TokenProperties& properties) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto notification = CreateNotification(owner, TokenId(123), properties);

			// - seed the cache with an active token with the same id
			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddEternalToken(cache, TokenId(123), owner);

			// Assert:
			AssertValidationResult(expectedResult, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CanReplaceActiveTokenWhenDefinitionIsChanged_Eternal) {
		AssertEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(1, 0, 0));
		AssertEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(0, 1, 0));
		AssertEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(1, 1, 0));
	}

	TEST(TEST_CLASS, CannotReplaceActiveTokenWhenDefinitionIsUnchanged_Eternal) {
		// Assert: duration delta is not a pertinent change
		AssertEternalPropertiesCheck(Failure_Token_Modification_No_Changes, test::CreateTokenPropertiesFromValues(0, 0, 1));
		AssertEternalPropertiesCheck(Failure_Token_Modification_No_Changes, test::CreateTokenPropertiesFromValues(0, 0, 0));
	}

	namespace {
		void AssertNonEternalPropertiesCheck(ValidationResult expectedResult, const model::TokenProperties& properties) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto notification = CreateNotification(owner, TokenId(123), properties);

			// - seed the cache with an active token with the same id and a lifetime of 100 blocks
			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), Amount(0), owner, Amount(0));

			// Assert:
			AssertValidationResult(expectedResult, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CanReplaceActiveTokenWhenDefinitionIsChanged_NonEternal) {
		AssertNonEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(1, 0, 0));
		AssertNonEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(0, 1, 0));
		AssertNonEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(0, 0, 1));
		AssertNonEternalPropertiesCheck(ValidationResult::Success, test::CreateTokenPropertiesFromValues(1, 1, 1));
	}

	TEST(TEST_CLASS, CannotReplaceActiveTokenWhenDefinitionIsUnchanged_NonEternal) {
		AssertNonEternalPropertiesCheck(Failure_Token_Modification_No_Changes, test::CreateTokenPropertiesFromValues(0, 0, 0));
	}

	// endregion

	// region supply check

	namespace {
		void AssertCanReplaceActiveTokenWhenSupplyIsZero(uint8_t divisibility) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto properties = test::CreateTokenPropertiesFromValues(0, divisibility, 200);
			auto notification = CreateNotification(owner, TokenId(123), properties);

			// - seed the cache with an active token with the same id and zero supply
			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), Amount(0), owner, Amount(0));

			// Assert:
			AssertValidationResult(ValidationResult::Success, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CanReplaceActiveTokenWhenSupplyIsZero_RequiredPropertiesChanged) {
		AssertCanReplaceActiveTokenWhenSupplyIsZero(3);
	}

	TEST(TEST_CLASS, CanReplaceActiveTokenWhenSupplyIsZero_RequiredPropertiesUnchanged) {
		AssertCanReplaceActiveTokenWhenSupplyIsZero(0);
	}

	TEST(TEST_CLASS, CannotReplaceActiveTokenWhenSupplyIsNonzero) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, 200);
		auto notification = CreateNotification(owner, TokenId(123), properties);

		// - seed the cache with an active token with the same id
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, TokenId(123), Amount(100), owner, Amount(100));

		// Assert:
		AssertValidationResult(Failure_Token_Modification_Disallowed, cache, Height(100), notification);
	}

	// endregion
}}
