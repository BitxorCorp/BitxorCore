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
#include "bitxorcore/cache_core/AccountStateCacheTypes.h"
#include "bitxorcore/cache_core/HighValueAccounts.h"

namespace bitxorcore { namespace test {

	/// Creates account state cache options for use in tests with \a currencyTokenId and \a harvestingTokenId.
	constexpr cache::AccountStateCacheTypes::Options CreateDefaultAccountStateCacheOptions(
			TokenId currencyTokenId,
			TokenId harvestingTokenId) {
		return {
			model::NetworkIdentifier::Testnet,
			333,
			222,
			Amount(),
			Amount(std::numeric_limits<Amount::ValueType>::max()),
			Amount(),
			currencyTokenId,
			harvestingTokenId
		};
	}

	/// Creates default account state cache options for use in tests.
	constexpr cache::AccountStateCacheTypes::Options CreateDefaultAccountStateCacheOptions() {
		return CreateDefaultAccountStateCacheOptions(TokenId(1111), TokenId(2222));
	}

	/// Creates an account history given the specified height and balance pairs (\a balancePairs).
	state::AccountHistory CreateAccountHistory(const std::vector<std::pair<Height, Amount>>& balancePairs);

	/// Balance seed data for generating an address account history map.
	using AddressBalanceHistorySeeds = std::vector<std::pair<Address, std::vector<std::pair<Height, Amount>>>>;

	/// Generates an address account history map from balance \a seeds.
	cache::AddressAccountHistoryMap GenerateAccountHistories(const AddressBalanceHistorySeeds& seeds);

	/// Adds random accounts to \a delta with specified \a balances of \a tokenId.
	std::vector<Address> AddAccountsWithBalances(
			cache::AccountStateCacheDelta& delta,
			TokenId tokenId,
			const std::vector<Amount>& balances);

	/// Adds random accounts to \a delta with specified \a balances of \a tokenId and overlapping voting key lifetimes.
	/// \note Accounts will have voting key lifetimes of [(10, 60), (20, 70) ...]
	std::vector<Address> AddAccountsWithBalancesAndOverlappingVotingKeyLifetimes(
			cache::AccountStateCacheDelta& delta,
			TokenId tokenId,
			const std::vector<Amount>& balances);

	/// Asserts that \a expected and \a actual are equal.
	void AssertEqual(const cache::AddressAccountHistoryMap& expected, const cache::AddressAccountHistoryMap& actual);

	/// Asserts that \a expected and \a actual have equal balance histories only.
	void AssertEqualBalanceHistoryOnly(const cache::AddressAccountHistoryMap& expected, const cache::AddressAccountHistoryMap& actual);
}}
