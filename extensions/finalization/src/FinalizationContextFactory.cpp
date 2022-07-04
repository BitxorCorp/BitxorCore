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

#include "FinalizationContextFactory.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/model/VotingSet.h"

namespace bitxorcore { namespace finalization {

	FinalizationContextFactory::FinalizationContextFactory(const FinalizationConfiguration& config, const extensions::ServiceState& state)
			: m_config(config)
			, m_accountStateCache(state.cache().sub<cache::AccountStateCache>())
			, m_blockStorage(state.storage())
	{}

	model::FinalizationContext FinalizationContextFactory::create(FinalizationEpoch epoch) const {
		if (FinalizationEpoch() == epoch)
			BITXORCORE_THROW_INVALID_ARGUMENT("epoch zero is not supported");

		auto votingSetHeight = model::CalculateVotingSetEndHeight(epoch - FinalizationEpoch(1), m_config.VotingSetGrouping);
		auto generationHash = m_blockStorage.view().loadBlockElement(votingSetHeight)->GenerationHash;

		BITXORCORE_LOG(trace) << "creating finalization context for epoch " << epoch << " with grouped height " << votingSetHeight;

		// when constructing FinalizationContext, to prevent possible deadlock, only have a single lock (to AccountStateCache) outstanding
		auto accountStateCacheView = m_accountStateCache.createView();
		return model::FinalizationContext(epoch, votingSetHeight, generationHash, m_config, *accountStateCacheView);
	}
}}
