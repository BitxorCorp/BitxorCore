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

#pragma once
#include "bitxorcore/model/Token.h"
#include <vector>

namespace bitxorcore {
	namespace cache {
		class AccountStateCacheDelta;
		class AccountStateCacheView;
		class BitxorCoreCacheDelta;
	}
}

namespace bitxorcore { namespace test {

	/// Vector of balance transfers.
	using BalanceTransfers = std::vector<model::Token>;

	/// Asserts that \a cache contains an account with \a publicKey that has all balances in \a expectedBalances.
	void AssertBalances(const cache::BitxorCoreCacheDelta& cache, const Key& publicKey, const BalanceTransfers& expectedBalances);

	/// Asserts that \a cache contains an account with \a address that has all balances in \a expectedBalances.
	void AssertBalances(const cache::BitxorCoreCacheDelta& cache, const Address& address, const BalanceTransfers& expectedBalances);

	/// Asserts that \a cache contains an account with \a publicKey that has all balances in \a expectedBalances.
	void AssertBalances(const cache::AccountStateCacheDelta& cache, const Key& publicKey, const BalanceTransfers& expectedBalances);

	/// Asserts that \a cache contains an account with \a publicKey that has all balances in \a expectedBalances.
	void AssertBalances(const cache::AccountStateCacheView& cache, const Key& publicKey, const BalanceTransfers& expectedBalances);
}}
