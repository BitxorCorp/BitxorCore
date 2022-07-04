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
#include "CompactTokenMap.h"
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/exceptions.h"
#include "bitxorcore/types.h"
#include <unordered_map>

namespace bitxorcore { namespace state {

	/// Container holding account balance information.
	class AccountBalances {
	public:
		/// Creates empty account balances.
		AccountBalances();

		/// Copy constructor that makes a deep copy of \a accountBalances.
		AccountBalances(const AccountBalances& accountBalances);

		/// Move constructor that move constructs an account balances from \a accountBalances.
		AccountBalances(AccountBalances&& accountBalances);

	public:
		/// Assignment operator that makes a deep copy of \a accountBalances.
		AccountBalances& operator=(const AccountBalances& accountBalances);

		/// Move assignment operator that assigns \a accountBalances.
		AccountBalances& operator=(AccountBalances&& accountBalances);

	public:
		/// Gets the number of tokens owned.
		size_t size() const;

		/// Gets a const iterator to the first element of the underlying set.
		CompactTokenMap::const_iterator begin() const;

		/// Gets a const iterator to the element following the last element of the underlying set.
		CompactTokenMap::const_iterator end() const;

		/// Gets the optimized token id.
		TokenId optimizedTokenId() const;

		/// Gets the balance of the given token (\a tokenId).
		Amount get(TokenId tokenId) const;

	public:
		/// Adds \a amount funds to a given token (\a tokenId).
		AccountBalances& credit(TokenId tokenId, Amount amount);

		/// Subtracts \a amount funds from a given token (\a tokenId).
		AccountBalances& debit(TokenId tokenId, Amount amount);

		/// Optimizes access of the token with \a id.
		void optimize(TokenId id);

	private:
		CompactTokenMap m_balances;
		TokenId m_optimizedTokenId;
	};
}}
