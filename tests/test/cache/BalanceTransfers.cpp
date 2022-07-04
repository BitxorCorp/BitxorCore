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

#include "BalanceTransfers.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	namespace {
		template<typename TCache, typename TAccountIdentifier>
		void AssertBalancesT(const TCache& cache, const TAccountIdentifier& accountIdentifier, const BalanceTransfers& expectedBalances) {
			// Assert:
			auto accountStateIter = cache.find(accountIdentifier);
			ASSERT_TRUE(!!accountStateIter.tryGet()) << accountIdentifier;

			const auto& accountState = accountStateIter.get();
			EXPECT_EQ(expectedBalances.size(), accountState.Balances.size());
			for (const auto& expectedBalance : expectedBalances) {
				BITXORCORE_LOG(debug) << expectedBalance.TokenId << " " << expectedBalance.Amount;
				EXPECT_EQ(expectedBalance.Amount, accountState.Balances.get(expectedBalance.TokenId))
						<< "token " << expectedBalance.TokenId;
			}
		}
	}

	void AssertBalances(const cache::BitxorCoreCacheDelta& cache, const Key& publicKey, const BalanceTransfers& expectedBalances) {
		// Assert:
		AssertBalancesT(cache.sub<cache::AccountStateCache>(), publicKey, expectedBalances);
	}

	void AssertBalances(const cache::BitxorCoreCacheDelta& cache, const Address& address, const BalanceTransfers& expectedBalances) {
		// Assert:
		AssertBalancesT(cache.sub<cache::AccountStateCache>(), address, expectedBalances);
	}

	void AssertBalances(const cache::AccountStateCacheDelta& cache, const Key& publicKey, const BalanceTransfers& expectedBalances) {
		// Assert:
		AssertBalancesT(cache, publicKey, expectedBalances);
	}

	void AssertBalances(const cache::AccountStateCacheView& cache, const Key& publicKey, const BalanceTransfers& expectedBalances) {
		// Assert:
		AssertBalancesT(cache, publicKey, expectedBalances);
	}
}}
