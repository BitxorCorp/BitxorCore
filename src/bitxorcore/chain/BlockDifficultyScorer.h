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
#include "bitxorcore/cache_core/BlockStatisticCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace chain {

	/// Calculates the block difficulty given the past difficulties and timestamps (\a statistics) for the
	/// blockchain described by \a config.
	Difficulty CalculateDifficulty(const cache::BlockStatisticRange& statistics, const model::BlockchainConfiguration& config);

	/// Calculates the block difficulty at \a height for the blockchain described by \a config
	/// given the block statistic \a cache.
	Difficulty CalculateDifficulty(const cache::BlockStatisticCache& cache, Height height, const model::BlockchainConfiguration& config);

	/// Calculates the block difficulty at \a height into \a difficulty for the blockchain described by
	/// \a config given the block statistic \a cache.
	bool TryCalculateDifficulty(
			const cache::BlockStatisticCache& cache,
			Height height,
			const model::BlockchainConfiguration& config,
			Difficulty& difficulty);
}}
