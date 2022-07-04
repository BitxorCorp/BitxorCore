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
#include "BitxorCoreCacheDetachedDelta.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace cache { class CacheHeightView; } }

namespace bitxorcore { namespace cache {

	/// Detachable delta of the bitxorcore cache.
	/// \note This delta acquires a reader lock (via the cache height view) to the bitxorcore cache that is released
	///       when the delta is destroyed.
	class BitxorCoreCacheDetachableDelta {
	public:
		/// Creates a detachable cache delta from a cache height view (\a cacheHeightView), \a dependentState and \a detachedSubViews.
		BitxorCoreCacheDetachableDelta(
				CacheHeightView&& cacheHeightView,
				const state::BitxorCoreState& dependentState,
				std::vector<std::unique_ptr<DetachedSubCacheView>>&& detachedSubViews);

		/// Destroys the detachable cache delta.
		~BitxorCoreCacheDetachableDelta();

	public:
		/// Move constructor.
		BitxorCoreCacheDetachableDelta(BitxorCoreCacheDetachableDelta&&);

	public:
		/// Gets the cache height associated with this delta.
		Height height() const;

		/// Detaches the detached delta from this delta.
		BitxorCoreCacheDetachedDelta detach();

	private:
		std::unique_ptr<const CacheHeightView> m_pCacheHeightView;
		BitxorCoreCacheDetachedDelta m_detachedDelta;
	};
}}
