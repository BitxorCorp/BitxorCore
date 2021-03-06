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

#include "Observers.h"
#include "bitxorcore/cache_core/BlockStatisticCache.h"
#include "bitxorcore/state/BlockStatistic.h"
#include "bitxorcore/state/DynamicFeeMultiplier.h"

namespace bitxorcore { namespace observers {

	DECLARE_OBSERVER(BlockStatistic, model::BlockNotification)(
			uint32_t maxDifficultyBlocks,
			BlockFeeMultiplier defaultDynamicFeeMultiplier) {
		return MAKE_OBSERVER(BlockStatistic, model::BlockNotification, ([maxDifficultyBlocks, defaultDynamicFeeMultiplier](
					const model::BlockNotification& notification,
					ObserverContext& context) {
			auto statistic = state::BlockStatistic(
					context.Height,
					notification.Timestamp,
					notification.Difficulty,
					notification.FeeMultiplier);
			auto& cache = context.Cache.sub<cache::BlockStatisticCache>();

			auto effectiveHeight = context.Height;
			if (NotifyMode::Commit == context.Mode) {
				cache.insert(statistic);
			} else {
				cache.remove(statistic);
				effectiveHeight = effectiveHeight - Height(1);
			}

			auto statistics = cache.statistics(effectiveHeight, maxDifficultyBlocks);
			context.Cache.dependentState().DynamicFeeMultiplier = state::CalculateDynamicFeeMultiplier(
					statistics.begin(),
					statistics.end(),
					maxDifficultyBlocks,
					defaultDynamicFeeMultiplier);
		}));
	}
}}
