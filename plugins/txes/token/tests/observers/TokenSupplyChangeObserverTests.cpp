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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace observers {

#define TEST_CLASS TokenSupplyChangeObserverTests

	using ObserverTestContext = test::ObserverTestContextT<test::TokenCacheFactory>;

	DEFINE_COMMON_OBSERVER_TESTS(TokenSupplyChange,)

	namespace {
		constexpr TokenId Default_Token_Id(345);

		void AssertSupplyChange(
				model::TokenSupplyChangeAction action,
				NotifyMode mode,
				Amount initialSupply,
				Amount initialOwnerSupply,
				Amount delta,
				Amount finalSupply,
				Amount finalOwnerSupply) {
			// Arrange: create observer and notification
			auto pObserver = CreateTokenSupplyChangeObserver();

			auto owner = test::CreateRandomOwner();
			model::TokenSupplyChangeNotification notification(owner, test::UnresolveXor(Default_Token_Id), action, delta);

			// - initialize cache with a token supply
			ObserverTestContext context(mode, Height(888));
			test::AddToken(context.cache(), Default_Token_Id, Height(7), Eternal_Artifact_Duration, initialSupply);
			test::AddTokenOwner(context.cache(), Default_Token_Id, owner, initialOwnerSupply);

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert:
			const auto& tokenCacheDelta = context.cache().sub<cache::TokenCache>();
			EXPECT_EQ(finalSupply, tokenCacheDelta.find(Default_Token_Id).get().supply());

			const auto& accountStateCacheDelta = context.cache().sub<cache::AccountStateCache>();
			auto ownerAddress = accountStateCacheDelta.find(owner).get().Address;
			EXPECT_EQ(finalOwnerSupply, accountStateCacheDelta.find(ownerAddress).get().Balances.get(Default_Token_Id));
		}

		void AssertSupplyIncrease(model::TokenSupplyChangeAction action, NotifyMode mode) {
			// Assert:
			AssertSupplyChange(action, mode, Amount(500), Amount(222), Amount(123), Amount(500 + 123), Amount(222 + 123));
		}

		void AssertSupplyDecrease(model::TokenSupplyChangeAction action, NotifyMode mode) {
			// Assert:
			AssertSupplyChange(action, mode, Amount(500), Amount(222), Amount(123), Amount(500 - 123), Amount(222 - 123));
		}
	}

	TEST(TEST_CLASS, IncreaseCommitIncreasesSupply) {
		AssertSupplyIncrease(model::TokenSupplyChangeAction::Increase, NotifyMode::Commit);
	}

	TEST(TEST_CLASS, DecreaseCommitDecreasesSupply) {
		AssertSupplyDecrease(model::TokenSupplyChangeAction::Decrease, NotifyMode::Commit);
	}

	TEST(TEST_CLASS, IncreaseRollbackDecreasesSupply) {
		AssertSupplyDecrease(model::TokenSupplyChangeAction::Increase, NotifyMode::Rollback);
	}

	TEST(TEST_CLASS, DecreaseRollbackIncreasesSupply) {
		AssertSupplyIncrease(model::TokenSupplyChangeAction::Decrease, NotifyMode::Rollback);
	}
}}
