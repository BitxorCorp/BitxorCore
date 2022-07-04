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

#include "GenesisBlockNotifier.h"
#include "bitxorcore/cache/CacheChanges.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/extensions/GenesisBlockLoader.h"
#include "bitxorcore/io/BlockChangeSubscriber.h"
#include "bitxorcore/io/BlockStatementSerializer.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/model/ChainScore.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/subscribers/FinalizationSubscriber.h"
#include "bitxorcore/subscribers/StateChangeInfo.h"
#include "bitxorcore/subscribers/StateChangeSubscriber.h"

namespace bitxorcore { namespace local {

	namespace {
		constexpr auto Genesis_Height = Height(1);
		constexpr auto Genesis_Chain_Score = static_cast<uint64_t>(1);

		bool HasPreviousExecution(const cache::BitxorCoreCache& cache) {
			// assume a previous execution if the account state cache is not empty
			// (this needs to be checked in order to prevent reexecution, which can lead to hash cache insert error)
			return 0 != cache.createView().sub<cache::AccountStateCache>().size();
		}
	}

	GenesisBlockNotifier::GenesisBlockNotifier(
			const model::BlockchainConfiguration& config,
			const cache::BitxorCoreCache& cache,
			const io::BlockStorageCache& storage,
			const plugins::PluginManager& pluginManager)
			: m_config(config)
			, m_cache(cache)
			, m_storage(storage)
			, m_pluginManager(pluginManager)
	{}

	void GenesisBlockNotifier::raise(io::BlockChangeSubscriber& subscriber) {
		raise([&subscriber](const auto& genesisBlockElement) {
			subscriber.notifyBlock(genesisBlockElement);
		});
	}

	void GenesisBlockNotifier::raise(subscribers::FinalizationSubscriber& subscriber) {
		raise([&subscriber](const auto& genesisBlockElement) {
			subscriber.notifyFinalizedBlock({ FinalizationEpoch(1), FinalizationPoint(1) }, Height(1), genesisBlockElement.EntityHash);
		});
	}

	void GenesisBlockNotifier::raise(subscribers::StateChangeSubscriber& subscriber) {
		raise([&subscriber, &config = m_config, &cache = m_cache, &pluginManager = m_pluginManager](const auto& genesisBlockElement) {
			// execute the genesis block
			auto cacheDetachableDelta = cache.createDetachableDelta();
			auto cacheDetachedDelta = cacheDetachableDelta.detach();
			auto pCacheDelta = cacheDetachedDelta.tryLock();

			extensions::GenesisBlockLoader loader(*pCacheDelta, pluginManager, pluginManager.createObserver());
			loader.execute(config, genesisBlockElement);

			// notify genesis cache state
			subscriber.notifyScoreChange(model::ChainScore(Genesis_Chain_Score));
			subscriber.notifyStateChange({
				cache::CacheChanges(*pCacheDelta),
				model::ChainScore::Delta(Genesis_Chain_Score),
				Genesis_Height
			});
		});
	}

	void GenesisBlockNotifier::raise(const consumer<model::BlockElement>& action) {
		// bypass if chain appears to have been previously executed
		auto storageView = m_storage.view();
		if (HasPreviousExecution(m_cache))
			BITXORCORE_THROW_RUNTIME_ERROR("GenesisBlockNotifier can only be called during first boot");

		auto pGenesisBlockElement = storageView.loadBlockElement(Genesis_Height);
		auto genesisBlockStatementPair = storageView.loadBlockStatementData(Genesis_Height);
		if (genesisBlockStatementPair.second) {
			auto pGenesisBlockStatement = std::make_shared<model::BlockStatement>();
			io::BufferInputStreamAdapter<std::vector<uint8_t>> genesisBlockStatementStream(genesisBlockStatementPair.first);
			io::ReadBlockStatement(genesisBlockStatementStream, *pGenesisBlockStatement);
			const_cast<model::BlockElement&>(*pGenesisBlockElement).OptionalStatement = std::move(pGenesisBlockStatement);
		}

		action(*pGenesisBlockElement);
	}
}}
