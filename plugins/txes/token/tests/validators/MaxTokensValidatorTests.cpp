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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

	DEFINE_COMMON_VALIDATOR_TESTS(MaxTokensBalanceTransfer, 123)
	DEFINE_COMMON_VALIDATOR_TESTS(MaxTokensSupplyChange, 123)

#define BALANCE_TRANSFER_TEST_CLASS BalanceTransferMaxTokensValidatorTests
#define SUPPLY_CHANGE_TEST_CLASS SupplyChangeMaxTokensValidatorTests

	namespace {
		template<typename TAccountIdentifier>
		auto CreateAndSeedCache(const TAccountIdentifier& accountIdentifier) {
			auto cache = test::CoreSystemCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
			{
				auto cacheDelta = cache.createDelta();
				auto& accountStateCacheDelta = cacheDelta.sub<cache::AccountStateCache>();
				accountStateCacheDelta.addAccount(accountIdentifier, Height());
				auto& accountState = accountStateCacheDelta.find(accountIdentifier).get();
				for (auto i = 0u; i < 5; ++i)
					accountState.Balances.credit(TokenId(i + 1), Amount(1));

				cache.commit(Height());
			}

			return cache;
		}

		void RunBalanceTransferTest(ValidationResult expectedResult, uint16_t maxTokens, UnresolvedTokenId tokenId, Amount amount) {
			// Arrange:
			auto recipient = test::GenerateRandomByteArray<Address>();
			auto unresolvedRecipient = test::UnresolveXor(recipient);
			auto cache = CreateAndSeedCache(recipient);

			auto pValidator = CreateMaxTokensBalanceTransferValidator(maxTokens);
			auto notification = model::BalanceTransferNotification(Address(), unresolvedRecipient, tokenId, amount);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result) << "maxTokens " << maxTokens << ", tokenId " << tokenId << ", amount " << amount;
		}
	}

	TEST(BALANCE_TRANSFER_TEST_CLASS, FailureWhenMaximumIsExceeded) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		RunBalanceTransferTest(Failure_Token_Max_Tokens_Exceeded, 1, test::UnresolveXor(TokenId(6)), Amount(100));
		RunBalanceTransferTest(Failure_Token_Max_Tokens_Exceeded, 4, test::UnresolveXor(TokenId(6)), Amount(100));
		RunBalanceTransferTest(Failure_Token_Max_Tokens_Exceeded, 5, test::UnresolveXor(TokenId(6)), Amount(100));
	}

	TEST(BALANCE_TRANSFER_TEST_CLASS, SuccessWhenAmountIsZero) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		RunBalanceTransferTest(ValidationResult::Success, 5, test::UnresolveXor(TokenId(6)), Amount(0));
	}

	TEST(BALANCE_TRANSFER_TEST_CLASS, SuccessWhenAccountAlreadyOwnsThatToken) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		RunBalanceTransferTest(ValidationResult::Success, 5, test::UnresolveXor(TokenId(3)), Amount(100));
	}

	TEST(BALANCE_TRANSFER_TEST_CLASS, SuccessWhenMaximumIsNotExceeded) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		RunBalanceTransferTest(ValidationResult::Success, 6, test::UnresolveXor(TokenId(6)), Amount(100));
		RunBalanceTransferTest(ValidationResult::Success, 10, test::UnresolveXor(TokenId(6)), Amount(100));
		RunBalanceTransferTest(ValidationResult::Success, 123, test::UnresolveXor(TokenId(6)), Amount(100));
	}

	namespace {
		void RunTokenSupplyTest(
				ValidationResult expectedResult,
				uint16_t maxTokens,
				UnresolvedTokenId tokenId,
				model::TokenSupplyChangeAction action) {
			// Arrange:
			auto owner = test::CreateRandomOwner();
			auto cache = CreateAndSeedCache(owner);

			auto pValidator = CreateMaxTokensSupplyChangeValidator(maxTokens);
			auto notification = model::TokenSupplyChangeNotification(owner, tokenId, action, Amount(100));

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result)
					<< "maxTokens " << maxTokens
					<< ", tokenId " << tokenId
					<< ", action " << utils::to_underlying_type(action);
		}
	}

	TEST(SUPPLY_CHANGE_TEST_CLASS, FailureWhenMaximumIsExceeded) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		auto action = model::TokenSupplyChangeAction::Increase;
		RunTokenSupplyTest(Failure_Token_Max_Tokens_Exceeded, 1, test::UnresolveXor(TokenId(6)), action);
		RunTokenSupplyTest(Failure_Token_Max_Tokens_Exceeded, 4, test::UnresolveXor(TokenId(6)), action);
		RunTokenSupplyTest(Failure_Token_Max_Tokens_Exceeded, 5, test::UnresolveXor(TokenId(6)), action);
	}

	TEST(SUPPLY_CHANGE_TEST_CLASS, SuccessWhenSupplyIsDecreased) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		auto action = model::TokenSupplyChangeAction::Decrease;
		RunTokenSupplyTest(ValidationResult::Success, 5, test::UnresolveXor(TokenId(6)), action);
	}

	TEST(SUPPLY_CHANGE_TEST_CLASS, SuccessWhenAccountAlreadyOwnsThatToken) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		auto action = model::TokenSupplyChangeAction::Increase;
		RunTokenSupplyTest(ValidationResult::Success, 5, test::UnresolveXor(TokenId(3)), action);
	}

	TEST(SUPPLY_CHANGE_TEST_CLASS, SuccessWhenMaximumIsNotExceeded) {
		// Act: account in test already owns 5 tokens with ids 1 to 5
		auto action = model::TokenSupplyChangeAction::Increase;
		RunTokenSupplyTest(ValidationResult::Success, 6, test::UnresolveXor(TokenId(6)), action);
		RunTokenSupplyTest(ValidationResult::Success, 10, test::UnresolveXor(TokenId(6)), action);
		RunTokenSupplyTest(ValidationResult::Success, 123, test::UnresolveXor(TokenId(6)), action);
	}
}}
