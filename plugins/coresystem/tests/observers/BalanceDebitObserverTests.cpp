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

#define TEST_CLASS BalanceDebitObserverTests

	DEFINE_COMMON_OBSERVER_TESTS(BalanceDebit,)

	namespace {
		template<typename TTraits>
		void AssertCommitObservation() {
			// Arrange:
			test::AccountObserverTestContext context(NotifyMode::Commit);
			auto pObserver = CreateBalanceDebitObserver();

			auto sender = test::GenerateRandomByteArray<Address>();
			auto notification = TTraits::CreateNotification(sender);

			test::SetCacheBalances(context.cache(), sender, TTraits::GetInitialSenderBalances());

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert:
			test::AssertBalances(context.cache(), sender, TTraits::GetFinalSenderBalances());
		}

		template<typename TTraits>
		void AssertRollbackObservation() {
			// Arrange:
			test::AccountObserverTestContext context(NotifyMode::Rollback);
			auto pObserver = CreateBalanceDebitObserver();

			auto sender = test::GenerateRandomByteArray<Address>();
			auto notification = TTraits::CreateNotification(sender);

			test::SetCacheBalances(context.cache(), sender, TTraits::GetFinalSenderBalances());

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert:
			test::AssertBalances(context.cache(), sender, TTraits::GetInitialSenderBalances());
		}
	}

#define DEFINE_BALANCE_OBSERVATION_TESTS(TEST_NAME) \
	TEST(TEST_CLASS, CanTransfer##TEST_NAME##_Commit) { AssertCommitObservation<TEST_NAME##Traits>(); } \
	TEST(TEST_CLASS, CanTransfer##TEST_NAME##_Rollback) { AssertRollbackObservation<TEST_NAME##Traits>(); }

	// region single token

	namespace {
		constexpr auto Currency_Token_Id = TokenId(1234);

		struct SingleTokenTraits {
			static auto CreateNotification(const Address& sender) {
				return model::BalanceDebitNotification(sender, test::UnresolveXor(Currency_Token_Id), Amount(234));
			}

			static test::BalanceTransfers GetInitialSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) } };
			}

			static test::BalanceTransfers GetFinalSenderBalances() {
				return { { Currency_Token_Id, Amount(1000 - 234) } };
			}
		};
	}

	DEFINE_BALANCE_OBSERVATION_TESTS(SingleToken)

	// endregion

	// region multiple tokens

	namespace {
		struct MultipleTokenTraits {
			static auto CreateNotification(const Address& sender) {
				return model::BalanceDebitNotification(sender, test::UnresolveXor(TokenId(12)), Amount(234));
			}

			static test::BalanceTransfers GetInitialSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) }, { TokenId(12), Amount(1200) } };
			}

			static test::BalanceTransfers GetFinalSenderBalances() {
				return { { Currency_Token_Id, Amount(1000) }, { TokenId(12), Amount(1200 - 234) } };
			}
		};
	}

	DEFINE_BALANCE_OBSERVATION_TESTS(MultipleToken)

	// endregion
}}
