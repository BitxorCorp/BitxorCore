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

#include "BlockStateHash.h"
#include "LocalTestUtils.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/chain/BlockExecutor.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/nodeps/Genesis.h"

namespace bitxorcore { namespace test {

	Hash256 CalculateGenesisStateHash(const model::BlockElement& blockElement, const config::BitxorCoreConfiguration& config) {
		auto pPluginManager = CreatePluginManagerWithRealPlugins(config);

		auto cache = pPluginManager->createCache();
		auto cacheDetachableDelta = cache.createDetachableDelta();
		auto cacheDetachedDelta = cacheDetachableDelta.detach();
		auto pCacheDelta = cacheDetachedDelta.tryLock();

		return CalculateBlockStateHash(blockElement.Block, *pCacheDelta, *pPluginManager);
	}

	Hash256 CalculateBlockStateHash(
			const model::Block& block,
			cache::BitxorCoreCacheDelta& cache,
			const plugins::PluginManager& pluginManager) {
		// 1. prepare observer
		observers::NotificationObserverAdapter entityObserver(pluginManager.createObserver(), pluginManager.createNotificationPublisher());

		// 2. prepare observer state
		auto observerState = observers::ObserverState(cache);

		// 3. prepare resolvers
		auto readOnlyCache = cache.toReadOnly();
		auto resolverContext = pluginManager.createResolverContext(readOnlyCache);

		// 4. execute block
		chain::ExecuteBlock(
				BlockToBlockElement(block, GetGenesisGenerationHashSeed()),
				{ entityObserver, resolverContext, observerState });
		return cache.calculateStateHash(block.Height).StateHash;
	}
}}
