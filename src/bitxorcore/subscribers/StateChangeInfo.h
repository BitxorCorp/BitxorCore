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
#include "bitxorcore/cache/CacheChanges.h"
#include "bitxorcore/model/ChainScore.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace subscribers {

	/// State change information.
	struct StateChangeInfo {
	public:
		/// Creates state change information around \a cacheChanges, \a scoreDelta and \a height.
		StateChangeInfo(cache::CacheChanges&& cacheChanges, model::ChainScore::Delta scoreDelta, Height height)
				: CacheChanges(std::move(cacheChanges))
				, ScoreDelta(scoreDelta)
				, Height(height)
		{}

	public:
		/// Cache changes.
		const cache::CacheChanges CacheChanges;

		/// Chain score delta.
		const model::ChainScore::Delta ScoreDelta;

		/// New chain height.
		const bitxorcore::Height Height;
	};
}}
