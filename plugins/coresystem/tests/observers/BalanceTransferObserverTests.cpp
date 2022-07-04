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

#include "src/observers/Observers.h"
#include "tests/test/cache/BalanceTransferTestUtils.h"
#include "tests/test/plugins/AccountObserverTestContext.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace observers {

#define TEST_CLASS BalanceTransferObserverTests

	DEFINE_COMMON_OBSERVER_TESTS(BalanceTransfer,)

	namespace {
		constexpr auto Dynamic_Fee_Multiplier = BlockFeeMultiplier(117);

		template<typename TTraits>
		void AssertCommitObservation() {
			// Arrange:
			test::AccountObserverTestContext context(NotifyMode::Commit);
			context.state().DynamicFeeMultiplier = Dynamic_Fee_Multiplier;
			auto pObserver = CreateBalanceTransferObserver();

			auto sender = test::GenerateRandomByteArray<Address>();
			auto recipient = test::GenerateRandomByteArray<Address>();
			auto notification = TTraits::CreateNotification(sender, test::UnresolveXor(recipient));

			test::SetCacheBalances(context.cache(), sender, TTraits::GetInitialSenderBalances());
			test::SetCacheBalances(context.cache(), recipient, TTraits::GetInitialRecipientBalances());

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert:
			test::AssertBalances(context.cache(), sender, TTraits::GetFinalSenderBalances());
			test::AssertBalances(context.cache(), recipient, TTraits::GetFinalRecipientBalances());
		}

		template<typename TTraits>
		void AssertRollbackObservation() {
			// Arrange:
			test::AccountObserverTestContext context(NotifyMode::Rollback);
			context.state().DynamicFeeMultiplier = Dynamic_Fee_Multiplier;
			auto pObserver = CreateBalanceTransferObserver();

			auto sender = test::GenerateRandomByteArray<Address>();
			auto recipient = test::GenerateRandomByteArray<Address>();
			auto notification = TTraits::CreateNotification(sender, test::UnresolveXor(recipient));

			test::SetCacheBalances(context.cache(), sender, TTraits::GetFinalSenderBalances());
			test::SetCacheBalances(context.cache(), recipient, TTraits::GetFinalRecipientBalances());

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert:
			test::AssertBalances(context.cache(), sender, TTraits::GetInitialSenderBalances());
			test::AssertBalances(context.cache(), recipient, TTraits::GetInitialRecipientBalances());
		}
	}

#define DEFINE_BALANCE_OBSERVATION_TESTS(TEST_NAME) \
	TEST(TEST_CLASS, CanTransfer##TEST_NAME##_Commit) { AssertCommitObservation<TEST_NAME##Traits>(); } \
	TEST(TEST_CLASS, CanTransfer##TEST_NAME##_Rollback) { AssertRollbackObservation<TEST_NAME##Traits>(); }

	// region single token

	namespace {
		constexpr auto Currency_Token_Id = TokenId(1234);

		struct SingleTokenStaticTraits {
			static auto CreateNotification(const Address& sender, const UnresolvedAddress& recipient) {
				return model::BalanceTransferNotification(sender, recipient, test::UnresolveXor(Currency_Token_Id), Amount(234));
			}

			static test::BalanceTransfers GetInitialSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) } };
			}

			static test::BalanceTransfers GetFinalSenderBalances() {
				return { { Currency_Token_Id, Amount(1000 - 234) } };
			}

			static test::BalanceTransfers GetInitialRecipientBalances() {
				return { { Currency_Token_Id, Amount(750) } };
			}

			static test::BalanceTransfers GetFinalRecipientBalances() {
				return { { Currency_Token_Id, Amount(750 + 234) } };
			}
		};

		struct SingleTokenDynamicTraits : public SingleTokenStaticTraits {
			static auto CreateNotification(const Address& sender, const UnresolvedAddress& recipient) {
				return model::BalanceTransferNotification(
						sender,
						recipient,
						test::UnresolveXor(Currency_Token_Id),
						Amount(234 / Dynamic_Fee_Multiplier.unwrap()),
						model::BalanceTransferNotification::AmountType::Dynamic);
			}
		};
	}

	DEFINE_BALANCE_OBSERVATION_TESTS(SingleTokenStatic)
	DEFINE_BALANCE_OBSERVATION_TESTS(SingleTokenDynamic)

	// endregion

	// region multiple tokens

	namespace {
		struct MultipleTokenStaticTraits {
			static auto CreateNotification(const Address& sender, const UnresolvedAddress& recipient) {
				return model::BalanceTransferNotification(sender, recipient, test::UnresolveXor(TokenId(12)), Amount(234));
			}

			static test::BalanceTransfers GetInitialSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) }, { TokenId(12), Amount(1200) } };
			}

			static test::BalanceTransfers GetFinalSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) }, { TokenId(12), Amount(1200 - 234) } };
			}

			static test::BalanceTransfers GetInitialRecipientBalances() {
				return { { Currency_Token_Id, Amount(750) }, { TokenId(12), Amount(500) } };
			}

			static test::BalanceTransfers GetFinalRecipientBalances() {
				return { { Currency_Token_Id, Amount(750) }, { TokenId(12), Amount(500 + 234) } };
			}
		};
	}

	struct MultipleTokenDynamicTraits : public MultipleTokenStaticTraits {
		static auto CreateNotification(const Address& sender, const UnresolvedAddress& recipient) {
			return model::BalanceTransferNotification(
					sender,
					recipient,
					test::UnresolveXor(TokenId(12)),
					Amount(234 / Dynamic_Fee_Multiplier.unwrap()),
					model::BalanceTransferNotification::AmountType::Dynamic);
		}
	};

	DEFINE_BALANCE_OBSERVATION_TESTS(MultipleTokenStatic)
	DEFINE_BALANCE_OBSERVATION_TESTS(MultipleTokenDynamic)

	// endregion
}}
