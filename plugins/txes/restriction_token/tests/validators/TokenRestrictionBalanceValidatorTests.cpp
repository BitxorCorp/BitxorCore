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
#include "tests/test/TokenRestrictionCacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRestrictionBalanceValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenRestrictionBalanceTransfer,)

	DEFINE_COMMON_VALIDATOR_TESTS(TokenRestrictionBalanceDebit,)

	// region traits

	namespace {
		constexpr auto Network_Identifier = static_cast<model::NetworkIdentifier>(18);

		struct TransferSenderTraits {
			static constexpr auto CreateValidator = CreateTokenRestrictionBalanceTransferValidator;

			static auto CreateNotification(const Address& address1, const Address& address2) {
				return model::BalanceTransferNotification(address1, test::UnresolveXor(address2), UnresolvedTokenId(), Amount());
			}
		};

		struct TransferRecipientTraits {
			static constexpr auto CreateValidator = CreateTokenRestrictionBalanceTransferValidator;

			static auto CreateNotification(const Address& address1, const Address& address2) {
				return model::BalanceTransferNotification(address2, test::UnresolveXor(address1), UnresolvedTokenId(), Amount());
			}
		};

		struct DebitSenderTraits {
			static constexpr auto CreateValidator = CreateTokenRestrictionBalanceDebitValidator;

			static auto CreateNotification(const Address& address1, const Address&) {
				return model::BalanceDebitNotification(address1, UnresolvedTokenId(), Amount());
			}
		};
	}

#define ACCOUNT_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TokenRestrictionBalanceTransferValidatorTests, TEST_NAME##_Sender) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TransferSenderTraits>(); \
	} \
	TEST(TokenRestrictionBalanceTransferValidatorTests, TEST_NAME##_Recipient) { \
		TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TransferRecipientTraits>(); \
	} \
	TEST(TokenRestrictionBalanceDebitValidatorTests, TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<DebitSenderTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// endregion

	// region tests

	namespace {
		static constexpr auto Token_Id_Invalid_Rule = TokenId(222);
		static constexpr auto Token_Id_Valid_Rule = TokenId(111);
		static constexpr auto Token_Id_No_Rules = TokenId(333);

		void SeedCacheForTests(
				cache::TokenRestrictionCacheDelta& delta,
				const std::vector<Address>& validAddresses,
				const std::vector<Address>& invalidAddresses) {
			// Arrange: invalid global restriction (self referential)
			auto restriction1 = state::TokenGlobalRestriction(Token_Id_Invalid_Rule);
			restriction1.set(200, { Token_Id_Invalid_Rule, 777, model::TokenRestrictionType::NE });
			delta.insert(state::TokenRestrictionEntry(restriction1));

			// - valid global restriction (EQ 888)
			auto restriction2 = state::TokenGlobalRestriction(Token_Id_Valid_Rule);
			restriction2.set(200, { TokenId(), 888, model::TokenRestrictionType::EQ });
			delta.insert(state::TokenRestrictionEntry(restriction2));

			// - valid addresses (EQ 888)
			for (const auto& address : validAddresses) {
				auto restriction = state::TokenAddressRestriction(Token_Id_Valid_Rule, address);
				restriction.set(200, 888);
				delta.insert(state::TokenRestrictionEntry(restriction));
			}

			// - invalid addresses (NE 888)
			for (const auto& address : invalidAddresses) {
				auto restriction = state::TokenAddressRestriction(Token_Id_Valid_Rule, address);
				restriction.set(200, 789);
				delta.insert(state::TokenRestrictionEntry(restriction));
			}
		}

		template<typename TTraits, typename TCacheSeeder>
		void RunTest(ValidationResult expectedResult, TokenId tokenId, TCacheSeeder seeder) {
			// Arrange:
			auto pValidator = TTraits::CreateValidator();

			auto address1 = test::GenerateRandomByteArray<Address>();
			auto address2 = test::GenerateRandomByteArray<Address>();
			auto notification = TTraits::CreateNotification(address1, address2);
			notification.TokenId = test::UnresolveXor(tokenId);
			notification.Amount = Amount(100);

			auto cache = test::TokenRestrictionCacheFactory::Create(Network_Identifier);
			{
				auto delta = cache.createDelta();
				seeder(delta.sub<cache::TokenRestrictionCache>(), address1, address2);
				cache.commit(Height());
			}

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	ACCOUNT_BASED_TEST(SuccessWhenNoTokenRulesAreConfigured) {
		auto expectedResult = ValidationResult::Success;
		RunTest<TTraits>(expectedResult, Token_Id_No_Rules, [](auto& cache, const auto&, const auto&) {
			SeedCacheForTests(cache, {}, {});
		});
	}

	ACCOUNT_BASED_TEST(FailureWhenInvalidTokenRulesAreConfigured) {
		auto expectedResult = Failure_RestrictionToken_Invalid_Global_Restriction;
		RunTest<TTraits>(expectedResult, Token_Id_Invalid_Rule, [](auto& cache, const auto& address1, const auto& address2) {
			SeedCacheForTests(cache, { address1, address2 }, {});
		});
	}

	ACCOUNT_BASED_TEST(FailureWhenValidTokenRulesAreConfiguredButOneAccountIsUnauthorizedDueToInvalidValues) {
		auto expectedResult = Failure_RestrictionToken_Account_Unauthorized;
		RunTest<TTraits>(expectedResult, Token_Id_Valid_Rule, [](auto& cache, const auto& address1, const auto& address2) {
			SeedCacheForTests(cache, { address2 }, { address1 });
		});
	}

	ACCOUNT_BASED_TEST(FailureWhenValidTokenRulesAreConfiguredButOneAccountIsUnauthorizedDueToUnsetValues) {
		auto expectedResult = Failure_RestrictionToken_Account_Unauthorized;
		RunTest<TTraits>(expectedResult, Token_Id_Valid_Rule, [](auto& cache, const auto&, const auto& address2) {
			SeedCacheForTests(cache, { address2 }, {});
		});
	}

	ACCOUNT_BASED_TEST(SuccessWhenValidTokenRulesAreConfiguredAndAllAccountsAreAuthorized) {
		auto expectedResult = ValidationResult::Success;
		RunTest<TTraits>(expectedResult, Token_Id_Valid_Rule, [](auto& cache, const auto& address1, const auto& address2) {
			SeedCacheForTests(cache, { address1, address2 }, {});
		});
	}

	// endregion
}}
