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
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenDurationValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenDuration, BlockDuration(123))

	namespace {
		constexpr TokenId Default_Token_Id = TokenId(0x1234);

		model::TokenDefinitionNotification CreateNotification(const Address& owner, const model::TokenProperties& properties) {
			return model::TokenDefinitionNotification(owner, Default_Token_Id, properties);
		}

		void AddToken(cache::BitxorCoreCache& cache, const Address& owner, BlockDuration duration) {
			auto delta = cache.createDelta();
			test::AddToken(delta, Default_Token_Id, Height(50), duration, owner);
			cache.commit(Height());
		}

		void AddEternalToken(cache::BitxorCoreCache& cache, const Address& owner) {
			auto delta = cache.createDelta();
			test::AddEternalToken(delta, Default_Token_Id, Height(50), owner);
			cache.commit(Height());
		}

		void AssertValidationResult(
				ValidationResult expectedResult,
				const cache::BitxorCoreCache& cache,
				const model::TokenDefinitionNotification& notification,
				Height height = Height(50)) {
			// Arrange:
			auto pValidator = CreateTokenDurationValidator(BlockDuration(123));

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache, height);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "id " << notification.TokenId;
		}
	}

	// region no duration change

	TEST(TEST_CLASS, SuccessWhenNonEternalTokenIsKnownAndDeltaIsZero) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, 0);
		auto notification = CreateNotification(owner, properties);

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, owner, BlockDuration(123));

		// Assert:
		AssertValidationResult(ValidationResult::Success, cache, notification);
	}

	TEST(TEST_CLASS, SuccessWhenEternalTokenIsKnownAndDeltaIsZero) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, 0);
		auto notification = CreateNotification(owner, properties);

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddEternalToken(cache, owner);

		// Assert:
		AssertValidationResult(ValidationResult::Success, cache, notification);
	}

	// endregion

	// region (new) unknown token

	TEST(TEST_CLASS, SuccessWhenTokenIsUnknownAndNotificationDurationDoesNotExceedMaxDuration) {
		// Arrange: create an empty cache
		auto owner = test::CreateRandomOwner();
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());

		// Assert: max duration is 123
		for (auto duration : { 1u, 70u, 123u }) {
			auto properties = test::CreateTokenPropertiesFromValues(0, 0, duration);
			AssertValidationResult(ValidationResult::Success, cache, CreateNotification(owner, properties));
		}
	}

	TEST(TEST_CLASS, FailureWhenTokenIsUnknownAndNotificationDurationExceedsMaxDuration) {
		// Arrange: create an empty cache
		auto owner = test::CreateRandomOwner();
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());

		// Assert: max duration is 123
		for (auto duration : { 124u, 999u }) {
			auto properties = test::CreateTokenPropertiesFromValues(0, 0, duration);
			AssertValidationResult(Failure_Token_Invalid_Duration, cache, CreateNotification(owner, properties));
		}
	}

	// endregion

	// region known token

	TEST(TEST_CLASS, FailureWhenChangingDurationFromEternalToNonEternal) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, 123);
		auto notification = CreateNotification(owner, properties);

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddEternalToken(cache, owner);

		// Assert:
		AssertValidationResult(Failure_Token_Invalid_Duration, cache, notification);
	}

	TEST(TEST_CLASS, FailureWhenResultingDurationExceedsMaxDuration) {
		// Arrange:
		auto owner = test::CreateRandomOwner();

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, owner, BlockDuration(100));

		// Assert: max duration is 123
		for (auto duration : { 24u, 25u, 999u }) {
			auto properties = test::CreateTokenPropertiesFromValues(0, 0, duration);
			AssertValidationResult(Failure_Token_Invalid_Duration, cache, CreateNotification(owner, properties));
		}
	}

	TEST(TEST_CLASS, FailureWhenDurationOverflowHappens) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesFromValues(0, 0, std::numeric_limits<uint64_t>::max() - 90);
		auto notification = CreateNotification(owner, properties);

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, owner, BlockDuration(100));

		// Assert:
		AssertValidationResult(Failure_Token_Invalid_Duration, cache, notification);
	}

	TEST(TEST_CLASS, SuccessWhenTokenIsKnownAndNewDurationIsAcceptable_NonEternal) {
		// Arrange:
		auto owner = test::CreateRandomOwner();

		// - seed the cache
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		AddToken(cache, owner, BlockDuration(100));

		// Assert:
		for (auto duration : { 1u, 22u, 23u }) {
			auto properties = test::CreateTokenPropertiesFromValues(0, 0, duration);
			AssertValidationResult(ValidationResult::Success, cache, CreateNotification(owner, properties));
		}
	}

	// endregion
}}
