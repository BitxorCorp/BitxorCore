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

#include "bitxorcore/extensions/GenesisFundingObserver.h"
#include "tests/test/core/AddressTestUtils.h"
#include "tests/test/plugins/AccountObserverTestContext.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS GenesisFundingObserverTests

	namespace {
		auto CreateBalanceTransferNotification(const Address& sender, UnresolvedTokenId tokenId, Amount amount) {
			return model::BalanceTransferNotification(sender, test::GenerateRandomUnresolvedAddress(), tokenId, amount);
		}
	}

	// region unsupported

	namespace {
		void AssertUnsupported(observers::NotifyMode notifyMode, Height height) {
			// Arrange:
			test::AccountObserverTestContext context(notifyMode, height);
			auto sender = test::GenerateRandomByteArray<Address>();

			GenesisFundingState fundingState;
			auto pObserver = CreateGenesisFundingObserver(sender, fundingState);
			auto notification = CreateBalanceTransferNotification(sender, UnresolvedTokenId(9876), Amount(12));

			// Act + Assert:
			EXPECT_THROW(test::ObserveNotification(*pObserver, notification, context.observerContext()), bitxorcore_invalid_argument);
		}
	}

	TEST(TEST_CLASS, ObserverDoesNotSupportRollback) {
		AssertUnsupported(observers::NotifyMode::Rollback, Height(1));
	}

	TEST(TEST_CLASS, ObserverDoesNotSupportNonGenesisBlock) {
		AssertUnsupported(observers::NotifyMode::Commit, Height(2));
	}

	// endregion

	// region non-genesis account

	TEST(TEST_CLASS, ObserverDoesNotFundNonGenesisAccount) {
		// Arrange:
		test::AccountObserverTestContext context(observers::NotifyMode::Commit, Height(1));
		auto genesis = test::GenerateRandomByteArray<Address>();
		auto sender = test::GenerateRandomByteArray<Address>();

		GenesisFundingState fundingState;
		auto pObserver = CreateGenesisFundingObserver(genesis, fundingState);
		auto notification = CreateBalanceTransferNotification(sender, UnresolvedTokenId(9876), Amount(12));

		// Act:
		test::ObserveNotification(*pObserver, notification, context.observerContext());

		// Assert: nothing was funded
		EXPECT_EQ(GenesisFundingType::Unknown, fundingState.FundingType);
		EXPECT_EQ(0u, fundingState.TotalFundedTokens.size());

		const auto& accountStateCache = context.cache().sub<cache::AccountStateCache>();
		EXPECT_EQ(0u, accountStateCache.size());
	}

	// endregion

	// region explicitly funded account

	TEST(TEST_CLASS, ObserverCanProcessExplicitlyFundedGenesisAccount) {
		// Arrange:
		auto tokenId1 = TokenId(1234);
		auto tokenId2 = TokenId(8844);

		// - pre-fund account
		test::AccountObserverTestContext context(observers::NotifyMode::Commit, Height(1));
		auto sender = test::GenerateRandomByteArray<Address>();

		auto& accountStateCache = context.cache().sub<cache::AccountStateCache>();
		accountStateCache.addAccount(sender, Height(1));
		accountStateCache.find(sender).get().Balances.credit(tokenId1, Amount(46));
		accountStateCache.find(sender).get().Balances.credit(tokenId2, Amount(99));

		GenesisFundingState fundingState;
		auto pObserver = CreateGenesisFundingObserver(sender, fundingState);
		auto notification1 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId1), Amount(12));
		auto notification2 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId1), Amount(34));
		auto notification3 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId2), Amount(39));
		auto notification4 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId2), Amount(60));

		// Act:
		test::ObserveNotification(*pObserver, notification1, context.observerContext()); // GenesisFundingType::Unknown
		test::ObserveNotification(*pObserver, notification2, context.observerContext()); // GenesisFundingType::Explicit
		test::ObserveNotification(*pObserver, notification3, context.observerContext()); // GenesisFundingType::Explicit
		test::ObserveNotification(*pObserver, notification4, context.observerContext()); // GenesisFundingType::Explicit

		// Assert:
		EXPECT_EQ(GenesisFundingType::Explicit, fundingState.FundingType);
		EXPECT_EQ(2u, fundingState.TotalFundedTokens.size());
		EXPECT_EQ(Amount(46), fundingState.TotalFundedTokens.get(tokenId1));
		EXPECT_EQ(Amount(99), fundingState.TotalFundedTokens.get(tokenId2));

		auto accountStateIter = accountStateCache.find(sender);
		const auto& balances = accountStateIter.get().Balances;
		EXPECT_EQ(1u, accountStateCache.size());
		EXPECT_EQ(2u, balances.size());
		EXPECT_EQ(Amount(46), balances.get(tokenId1));
		EXPECT_EQ(Amount(99), balances.get(tokenId2));
	}

	// endregion

	// region implicitly funded account

	TEST(TEST_CLASS, ObserverCanProcessImplicitlyFundedGenesisAccount) {
		// Arrange:
		auto tokenId1 = TokenId(1234);
		auto tokenId2 = TokenId(8844);

		// - don't pre-fund account
		test::AccountObserverTestContext context(observers::NotifyMode::Commit, Height(1));
		auto sender = test::GenerateRandomByteArray<Address>();

		GenesisFundingState fundingState;
		auto pObserver = CreateGenesisFundingObserver(sender, fundingState);
		auto notification1 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId1), Amount(12));
		auto notification2 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId1), Amount(34));
		auto notification3 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId2), Amount(39));
		auto notification4 = CreateBalanceTransferNotification(sender, test::UnresolveXor(tokenId2), Amount(60));

		// Act:
		test::ObserveNotification(*pObserver, notification1, context.observerContext()); // GenesisFundingType::Unknown
		test::ObserveNotification(*pObserver, notification2, context.observerContext()); // GenesisFundingType::Implicit
		test::ObserveNotification(*pObserver, notification3, context.observerContext()); // GenesisFundingType::Implicit
		test::ObserveNotification(*pObserver, notification4, context.observerContext()); // GenesisFundingType::Implicit

		// Assert:
		EXPECT_EQ(GenesisFundingType::Implicit, fundingState.FundingType);
		EXPECT_EQ(2u, fundingState.TotalFundedTokens.size());
		EXPECT_EQ(Amount(46), fundingState.TotalFundedTokens.get(tokenId1));
		EXPECT_EQ(Amount(99), fundingState.TotalFundedTokens.get(tokenId2));

		auto& accountStateCache = context.cache().sub<cache::AccountStateCache>();
		auto accountStateIter = accountStateCache.find(sender);
		const auto& balances = accountStateIter.get().Balances;
		EXPECT_EQ(1u, accountStateCache.size());
		EXPECT_EQ(2u, balances.size());
		EXPECT_EQ(Amount(46), balances.get(tokenId1));
		EXPECT_EQ(Amount(99), balances.get(tokenId2));
	}

	// endregion
}}
