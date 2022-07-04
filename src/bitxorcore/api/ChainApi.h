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
#include "ApiTypes.h"
#include "bitxorcore/model/ChainScore.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/thread/Future.h"
#include "bitxorcore/utils/NonCopyable.h"

namespace bitxorcore { namespace api {

	/// Chain statistics.
	struct ChainStatistics {
		/// Chain height.
		bitxorcore::Height Height;

		/// Finalized chain height.
		bitxorcore::Height FinalizedHeight;

		/// Chain score.
		model::ChainScore Score;
	};

	/// Api for retrieving chain statistics from a node.
	class ChainApi : public utils::NonCopyable {
	public:
		virtual ~ChainApi() = default;

	public:
		/// Gets the chain statistics.
		virtual thread::future<ChainStatistics> chainStatistics() const = 0;

		/// Gets the hashes starting at \a height but no more than \a maxHashes.
		virtual thread::future<model::HashRange> hashesFrom(Height height, uint32_t maxHashes) const = 0;
	};
}}
