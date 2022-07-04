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
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenSupplyChangeAllowedValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenSupplyChangeAllowed, Amount())

	namespace {
		constexpr auto Max_Atomic_Units = Amount(std::numeric_limits<Amount::ValueType>::max());

		void AssertValidationResult(
				ValidationResult expectedResult,
				const cache::BitxorCoreCache& cache,
				Height height,
				const model::TokenSupplyChangeNotification& notification,
				Amount maxAtomicUnits = Max_Atomic_Units) {
			// Arrange:
			auto pValidator = CreateTokenSupplyChangeAllowedValidator(maxAtomicUnits);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache, height);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "id " << notification.TokenId << ", delta " << notification.Delta;
		}

		void AddToken(
				cache::BitxorCoreCache& cache,
				TokenId id,
				Amount tokenSupply,
				const Address& owner,
				Amount ownerSupply,
				model::TokenFlags flags = model::TokenFlags::Supply_Mutable) {
			auto delta = cache.createDelta();

			// add a token definition with the desired flags
			model::TokenProperties properties(flags, 0, BlockDuration());

			auto& tokenCacheDelta = delta.sub<cache::TokenCache>();
			auto definition = state::TokenDefinition(Height(50), owner, 3, properties);
			auto entry = state::TokenEntry(id, definition);
			entry.increaseSupply(tokenSupply);
			tokenCacheDelta.insert(entry);

			test::AddTokenOwner(delta, id, owner, ownerSupply);
			cache.commit(Height());
		}
	}

	// region immutable supply

	namespace {
		void AssertCanChangeImmutableSupplyWhenOwnerHasCompleteSupply(model::TokenSupplyChangeAction action) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto notification = model::TokenSupplyChangeNotification(owner, test::UnresolveXor(TokenId(123)), action, Amount(100));

			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), Amount(500), owner, Amount(500), model::TokenFlags::None);

			// Assert:
			AssertValidationResult(ValidationResult::Success, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CanIncreaseImmutableSupplyWhenOwnerHasCompleteSupply) {
		AssertCanChangeImmutableSupplyWhenOwnerHasCompleteSupply(model::TokenSupplyChangeAction::Increase);
	}

	TEST(TEST_CLASS, CanDecreaseImmutableSupplyWhenOwnerHasCompleteSupply) {
		AssertCanChangeImmutableSupplyWhenOwnerHasCompleteSupply(model::TokenSupplyChangeAction::Decrease);
	}

	namespace {
		void AssertCannotChangeImmutableSupplyWhenOwnerHasPartialSupply(model::TokenSupplyChangeAction action) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto notification = model::TokenSupplyChangeNotification(owner, test::UnresolveXor(TokenId(123)), action, Amount(100));

			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), Amount(500), owner, Amount(499), model::TokenFlags::None);

			// Assert:
			AssertValidationResult(Failure_Token_Supply_Immutable, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CannotIncreaseImmutableSupplyWhenOwnerHasPartialSupply) {
		AssertCannotChangeImmutableSupplyWhenOwnerHasPartialSupply(model::TokenSupplyChangeAction::Increase);
	}

	TEST(TEST_CLASS, CannotDecreaseImmutableSupplyWhenOwnerHasPartialSupply) {
		AssertCannotChangeImmutableSupplyWhenOwnerHasPartialSupply(model::TokenSupplyChangeAction::Decrease);
	}

	// endregion

	// region decrease supply

	namespace {
		void AssertDecreaseValidationResult(ValidationResult expectedResult, Amount tokenSupply, Amount ownerSupply, Amount delta) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto action = model::TokenSupplyChangeAction::Decrease;
			auto notification = model::TokenSupplyChangeNotification(owner, test::UnresolveXor(TokenId(123)), action, delta);

			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), tokenSupply, owner, ownerSupply);

			// Assert:
			AssertValidationResult(expectedResult, cache, Height(100), notification);
		}
	}

	TEST(TEST_CLASS, CanDecreaseMutableSupplyByLessThanOwnerSupply) {
		AssertDecreaseValidationResult(ValidationResult::Success, Amount(500), Amount(400), Amount(300));
		AssertDecreaseValidationResult(ValidationResult::Success, Amount(500), Amount(400), Amount(399));
	}

	TEST(TEST_CLASS, CanDecreaseMutableSupplyByEntireOwnerSupply) {
		AssertDecreaseValidationResult(ValidationResult::Success, Amount(500), Amount(400), Amount(400));
	}

	TEST(TEST_CLASS, CannotDecreaseMutableSupplyByGreaterThanOwnerSupply) {
		AssertDecreaseValidationResult(Failure_Token_Supply_Negative, Amount(500), Amount(400), Amount(401));
		AssertDecreaseValidationResult(Failure_Token_Supply_Negative, Amount(500), Amount(400), Amount(500));
	}

	// endregion

	// region increase

	namespace {
		void AssertIncreaseValidationResult(ValidationResult expectedResult, Amount maxAtomicUnits, Amount tokenSupply, Amount delta) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto action = model::TokenSupplyChangeAction::Increase;
			auto notification = model::TokenSupplyChangeNotification(owner, test::UnresolveXor(TokenId(123)), action, delta);

			auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			AddToken(cache, TokenId(123), tokenSupply, owner, Amount(111));

			// Assert:
			AssertValidationResult(expectedResult, cache, Height(100), notification, maxAtomicUnits);
		}
	}

	TEST(TEST_CLASS, CanIncreaseMutableSupplyToLessThanAtomicUnits) {
		AssertIncreaseValidationResult(ValidationResult::Success, Amount(900), Amount(500), Amount(300));
		AssertIncreaseValidationResult(ValidationResult::Success, Amount(900), Amount(500), Amount(399));
	}

	TEST(TEST_CLASS, CanIncreaseMutableSupplyToExactlyAtomicUnits) {
		AssertIncreaseValidationResult(ValidationResult::Success, Amount(900), Amount(500), Amount(400));
	}

	TEST(TEST_CLASS, CannotIncreaseMutableSupplyToGreaterThanAtomicUnits) {
		AssertIncreaseValidationResult(Failure_Token_Supply_Exceeded, Amount(900), Amount(500), Amount(401));
		AssertIncreaseValidationResult(Failure_Token_Supply_Exceeded, Amount(900), Amount(500), Amount(500));
	}

	TEST(TEST_CLASS, CannotIncreaseMutableSupplyWhenOverflowIsDetected) {
		AssertIncreaseValidationResult(Failure_Token_Supply_Exceeded, Amount(900), Amount(500), Max_Atomic_Units);
	}

	// endregion
}}
