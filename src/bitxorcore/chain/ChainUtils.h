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
#include "bitxorcore/model/Block.h"
#include "bitxorcore/model/ChainScore.h"
#include <vector>

namespace bitxorcore {
	namespace cache { class BlockStatisticCache; }
	namespace model { struct BlockchainConfiguration; }
}

namespace bitxorcore { namespace chain {

	/// Determines if \a parent with hash \a parentHash and \a child form a chain link.
	bool IsChainLink(const model::Block& parent, const Hash256& parentHash, const model::Block& child);

	/// Checks if the difficulties in \a blocks are consistent with the difficulties stored in \a cache
	/// for the blockchain described by \a config. If there is an inconsistency, the index of the first
	/// difference is returned. Otherwise, the size of \a blocks is returned.
	size_t CheckDifficulties(
			const cache::BlockStatisticCache& cache,
			const std::vector<const model::Block*>& blocks,
			const model::BlockchainConfiguration& config);

	/// Calculates the partial chain score of \a blocks starting at \a parent.
	model::ChainScore CalculatePartialChainScore(const model::Block& parent, const std::vector<const model::Block*>& blocks);
}}
