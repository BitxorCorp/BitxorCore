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
#include "tests/test/AccountRestrictionCacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRecipientValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenRecipient,)

	namespace {
		template<typename TOperationTraits>
		void PopulateCache(cache::BitxorCoreCache& cache, const Address& accountAddress, const std::vector<TokenId>& tokenIds) {
			auto delta = cache.createDelta();
			auto& restrictionCacheDelta = delta.sub<cache::AccountRestrictionCache>();
			restrictionCacheDelta.insert(state::AccountRestrictions(accountAddress));
			auto& restrictions = restrictionCacheDelta.find(accountAddress).get();
			auto& restriction = restrictions.restriction(model::AccountRestrictionFlags::TokenId);
			for (auto tokenId : tokenIds)
				TOperationTraits::Add(restriction, state::ToVector(tokenId));

			cache.commit(Height(1));
		}

		template<typename TOperationTraits>
		void AssertValidationResult(
				ValidationResult expectedResult,
				const Address& accountAddress,
				const std::vector<TokenId>& tokenIds,
				const UnresolvedAddress& recipient,
				UnresolvedTokenId tokenId) {
			// Arrange:
			auto cache = test::AccountRestrictionCacheFactory::Create();
			PopulateCache<TOperationTraits>(cache, accountAddress, tokenIds);
			auto pValidator = CreateTokenRecipientValidator();
			auto sender = test::GenerateRandomByteArray<Address>();
			auto notification = model::BalanceTransferNotification(sender, recipient, tokenId, Amount(123));

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	// region failure

	TEST(TEST_CLASS, FailureWhenRecipientIsKnownAndTokenIdIsNotContainedInValues_Allow) {
		// Arrange:
		auto accountAddress = test::GenerateRandomByteArray<Address>();

		// Act:
		AssertValidationResult<test::AllowTraits>(
				Failure_RestrictionAccount_Token_Transfer_Prohibited,
				accountAddress,
				test::GenerateRandomDataVector<TokenId>(3),
				test::UnresolveXor(accountAddress),
				test::GenerateRandomValue<UnresolvedTokenId>());
	}

	TEST(TEST_CLASS, FailureWhenRecipientIsKnownAndTokenIdIsContainedInValues_Block) {
		// Arrange:
		auto accountAddress = test::GenerateRandomByteArray<Address>();
		auto values = test::GenerateRandomDataVector<TokenId>(3);

		// Act:
		AssertValidationResult<test::BlockTraits>(
				Failure_RestrictionAccount_Token_Transfer_Prohibited,
				accountAddress,
				values,
				test::UnresolveXor(accountAddress),
				test::UnresolveXor(values[1]));
	}

	// endregion

	// region success

#define TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Allow) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<test::AllowTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Block) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<test::BlockTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	TRAITS_BASED_TEST(SuccessWhenRecipientIsNotKnown) {
		// Arrange:
		auto accountAddress = test::GenerateRandomByteArray<Address>();

		// Act:
		AssertValidationResult<TTraits>(
				ValidationResult::Success,
				accountAddress,
				test::GenerateRandomDataVector<TokenId>(3),
				test::UnresolveXor(test::GenerateRandomByteArray<Address>()),
				test::GenerateRandomValue<UnresolvedTokenId>());
	}

	TRAITS_BASED_TEST(SuccessWhenRecipientIsKnownButAccountRestrictionHasNoValues) {
		// Arrange:
		auto accountAddress = test::GenerateRandomByteArray<Address>();

		// Act:
		AssertValidationResult<TTraits>(
				ValidationResult::Success,
				accountAddress,
				test::GenerateRandomDataVector<TokenId>(0),
				test::UnresolveXor(accountAddress),
				test::GenerateRandomValue<UnresolvedTokenId>());
	}

	namespace {
		template<typename TOperationTraits>
		void AssertSuccess(const std::vector<TokenId>& tokenIds, UnresolvedTokenId transferredTokenId) {
			// Arrange:
			auto accountAddress = test::GenerateRandomByteArray<Address>();

			// Act:
			AssertValidationResult<TOperationTraits>(
					ValidationResult::Success,
					accountAddress,
					tokenIds,
					test::UnresolveXor(accountAddress),
					transferredTokenId);
		}
	}

	TEST(TEST_CLASS, SuccessWhenAllConditionsAreMet_Allow) {
		// Arrange:
		auto tokenIds = test::GenerateRandomDataVector<TokenId>(3);

		// Act:
		AssertSuccess<test::AllowTraits>(tokenIds, test::UnresolveXor(tokenIds[1]));
	}

	TEST(TEST_CLASS, SuccessWhenAllConditionsAreMet_Block) {
		// Act:
		AssertSuccess<test::BlockTraits>(test::GenerateRandomDataVector<TokenId>(3), test::GenerateRandomValue<UnresolvedTokenId>());
	}

	// endregion
}}
