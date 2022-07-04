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
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/chain/ExecutionConfiguration.h"
#include "bitxorcore/model/Block.h"
#include "bitxorcore/model/BlockchainConfiguration.h"

namespace bitxorcore { namespace harvesting {

	/// Importance block hash supplier.
	using ImportanceBlockHashSupplier = std::function<Hash256 (Height)>;

	/// Facade around unconfirmed transactions cache and updater.
	class HarvestingUtFacade {
	public:
		/// Creates a facade around \a blockTime, \a cache, \a blockchainConfig, \a executionConfig and \a importanceBlockHashSupplier.
		HarvestingUtFacade(
				Timestamp blockTime,
				const cache::BitxorCoreCache& cache,
				const model::BlockchainConfiguration& blockchainConfig,
				const chain::ExecutionConfiguration& executionConfig,
				const ImportanceBlockHashSupplier& importanceBlockHashSupplier);

		/// Destroys the facade.
		~HarvestingUtFacade();

	public:
		/// Gets the locked height.
		Height height() const;

		/// Gets the number of successfully applied transactions.
		size_t size() const;

		/// Gets all successfully applied transactions.
		const std::vector<model::TransactionInfo>& transactionInfos() const;

	public:
		/// Attempts to apply \a transactionInfo to the cache.
		bool apply(const model::TransactionInfo& transactionInfo);

		/// Unapplies last successfully applied transaction.
		void unapply();

		/// Commits all transactions into a block with specified seed header (\a blockHeader).
		std::unique_ptr<model::Block> commit(const model::BlockHeader& blockHeader);

	private:
		class Impl;

	private:
		std::vector<model::TransactionInfo> m_transactionInfos;
		std::unique_ptr<Impl> m_pImpl;
	};

	/// Factory for creating unconfirmed transactions facades.
	class HarvestingUtFacadeFactory {
	public:
		/// Creates a factory around \a cache, \a blockchainConfig, \a executionConfig and \a importanceBlockHashSupplier.
		HarvestingUtFacadeFactory(
				const cache::BitxorCoreCache& cache,
				const model::BlockchainConfiguration& blockchainConfig,
				const chain::ExecutionConfiguration& executionConfig,
				const ImportanceBlockHashSupplier& importanceBlockHashSupplier);

	public:
		/// Creates a facade for applying transactions at a given block time (\a blockTime).
		std::unique_ptr<HarvestingUtFacade> create(Timestamp blockTime) const;

	private:
		const cache::BitxorCoreCache& m_cache;
		model::BlockchainConfiguration m_blockchainConfig;
		chain::ExecutionConfiguration m_executionConfig;
		ImportanceBlockHashSupplier m_importanceBlockHashSupplier;
	};
}}
