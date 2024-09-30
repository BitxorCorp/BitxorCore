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

#include "GenesisBlockHashesCalculator.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/chain/BlockExecutor.h"
#include "bitxorcore/model/GenesisNotificationPublisher.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace tools { namespace bxorgen {

	BlockExecutionHashesInfo CalculateGenesisBlockExecutionHashes(
			const model::BlockElement& blockElement,
			const model::BlockchainConfiguration& config,
			plugins::PluginManager& pluginManager) {
		// 1. prepare observer
		auto publisherOptions = model::ExtractGenesisNotificationPublisherOptions(config);
		observers::NotificationObserverAdapter entityObserver(
				pluginManager.createObserver(),
				model::CreateGenesisNotificationPublisher(pluginManager.createNotificationPublisher(), publisherOptions));

		// 2. prepare observer state
		auto cache = pluginManager.createCache();
		auto cacheDetachableDelta = cache.createDetachableDelta();
		auto cacheDetachedDelta = cacheDetachableDelta.detach();
		auto pCacheDelta = cacheDetachedDelta.tryLock();
		auto blockStatementBuilder = model::BlockStatementBuilder();
		auto observerState = observers::ObserverState(*pCacheDelta, blockStatementBuilder);

		// 3. prepare resolvers
		auto readOnlyCache = pCacheDelta->toReadOnly();
		auto resolverContext = pluginManager.createResolverContext(readOnlyCache);

		// 4. execute block
		chain::ExecuteBlock(blockElement, { entityObserver, resolverContext, observerState });
		auto pBlockStatement = blockStatementBuilder.build();
		auto cacheStateHashInfo = pCacheDelta->calculateStateHash(blockElement.Block.Height);
		auto blockReceiptsHash = config.EnableVerifiableReceipts
				? model::CalculateMerkleHash(*pBlockStatement)
				: Hash256();

		auto highValueAccountStatistics = readOnlyCache.sub<cache::AccountStateCache>().highValueAccountStatistics(FinalizationEpoch(0));
		return {
			highValueAccountStatistics.VotingEligibleAccountsCount,
			highValueAccountStatistics.HarvestingEligibleAccountsCount,
			highValueAccountStatistics.TotalVotingBalance,
			blockReceiptsHash,
			cacheStateHashInfo.StateHash,
			cacheStateHashInfo.SubCacheMerkleRoots,
			std::move(pBlockStatement)
		};
	}
}}}
