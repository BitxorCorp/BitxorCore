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

#include "StateChangeRepairingSubscriber.h"
#include "bitxorcore/cache/CacheChangesStorage.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/subscribers/StateChangeInfo.h"

namespace bitxorcore { namespace local {

	namespace {
		class StateChangeRepairingSubscriber : public subscribers::StateChangeSubscriber {
		public:
			StateChangeRepairingSubscriber(cache::BitxorCoreCache& cache, extensions::LocalNodeChainScore& localNodeScore)
					: m_cache(cache)
					, m_localNodeScore(localNodeScore)
			{}

		public:
			void notifyScoreChange(const model::ChainScore& chainScore) override {
				m_localNodeScore.set(chainScore);
			}

			void notifyStateChange(const subscribers::StateChangeInfo& stateChangeInfo) override {
				const auto& cacheChanges = stateChangeInfo.CacheChanges;
				auto changesStorages = m_cache.changesStorages();
				for (const auto& pStorage : changesStorages)
					pStorage->apply(cacheChanges);

				auto delta = m_cache.createDelta();
				m_cache.commit(stateChangeInfo.Height);
			}

		private:
			cache::BitxorCoreCache& m_cache;
			extensions::LocalNodeChainScore& m_localNodeScore;
		};
	}

	std::unique_ptr<subscribers::StateChangeSubscriber> CreateStateChangeRepairingSubscriber(
			cache::BitxorCoreCache& cache,
			extensions::LocalNodeChainScore& localNodeScore) {
		return std::make_unique<StateChangeRepairingSubscriber>(cache, localNodeScore);
	}
}}
