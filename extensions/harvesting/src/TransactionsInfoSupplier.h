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
#include "bitxorcore/cache_tx/MemoryUtCacheUtils.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/model/TransactionSelectionStrategy.h"

namespace bitxorcore { namespace harvesting { class HarvestingUtFacade; } }

namespace bitxorcore { namespace harvesting {

	/// Information about transactions.
	struct TransactionsInfo {
		/// Optimal fee multiplier.
		BlockFeeMultiplier FeeMultiplier;

		/// Transactions (shared pointers) (ordered).
		model::Transactions Transactions;

		/// Transaction hashes (ordered).
		std::vector<Hash256> TransactionHashes;

		/// Aggregate transactions hash.
		Hash256 TransactionsHash;
	};

	/// Supplies a transactions info composed of a maximum number of transactions for a block given a harvesting ut facade.
	using TransactionsInfoSupplier = std::function<TransactionsInfo (HarvestingUtFacade&, uint32_t)>;

	/// Creates a default transactions info supplier around \a utCache for specified transaction \a strategy
	/// where \a countRetriever returns the total number of transactions contained within a top-level transaction.
	TransactionsInfoSupplier CreateTransactionsInfoSupplier(
			model::TransactionSelectionStrategy strategy,
			const cache::EmbeddedCountRetriever& countRetriever,
			const cache::ReadWriteUtCache& utCache);
}}
