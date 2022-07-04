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
#include "bitxorcore/chain/UtUpdater.h"
#include "bitxorcore/utils/FileSize.h"

namespace bitxorcore { namespace sync {

	/// Spam throttle configuration.
	struct SpamThrottleConfiguration {
	public:
		/// Creates a default spam throttle configuration.
		SpamThrottleConfiguration()
				: TotalImportance(0)
				, MaxTransactionsPerBlock(0)
		{}

		/// Creates a spam throttle configuration around \a maxBoostFee, \a totalImportance, \a maxCacheSize
		/// and \a maxTransactionsPerBlock.
		SpamThrottleConfiguration(
				Amount maxBoostFee,
				Importance totalImportance,
				utils::FileSize maxCacheSize,
				uint32_t maxTransactionsPerBlock)
				: MaxBoostFee(maxBoostFee)
				, TotalImportance(totalImportance)
				, MaxCacheSize(maxCacheSize)
				, MaxTransactionsPerBlock(maxTransactionsPerBlock)
		{}

	public:
		/// Max fee for boosting importance.
		Amount MaxBoostFee;

		/// Total importance of all accounts.
		Importance TotalImportance;

		/// Maximum transactions cache size (in bytes).
		utils::FileSize MaxCacheSize;

		/// Maximum number of transactions per block (count).
		uint32_t MaxTransactionsPerBlock;
	};

	/// Creates a throttle using \a config to filter out transactions that are considered to be spam.
	/// \a isBonded indicates whether a transaction is bonded or not.
	chain::UtUpdater::Throttle CreateTransactionSpamThrottle(
			const SpamThrottleConfiguration& config,
			const predicate<const model::Transaction&>& isBonded);
}}
