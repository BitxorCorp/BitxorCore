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
#include "BitxorCoreCacheDelta.h"
#include <vector>

namespace bitxorcore { namespace cache { class DetachedSubCacheView; } }

namespace bitxorcore { namespace cache {

	/// Detached delta of the bitxorcore cache.
	class BitxorCoreCacheDetachedDelta {
	public:
		/// Creates a detached cache delta from \a dependentState and \a detachedSubViews.
		BitxorCoreCacheDetachedDelta(
				const state::BitxorCoreState& dependentState,
				std::vector<std::unique_ptr<DetachedSubCacheView>>&& detachedSubViews);

		/// Destroys the delta.
		~BitxorCoreCacheDetachedDelta();

	public:
		// make this class move only (the definitions are in the source file in order to allow forward declarations)
		BitxorCoreCacheDetachedDelta(BitxorCoreCacheDetachedDelta&&);
		BitxorCoreCacheDetachedDelta& operator=(BitxorCoreCacheDetachedDelta&&);

	public:
		/// Locks the detached delta.
		/// \note If locking fails, \c nullptr is returned.
		std::unique_ptr<BitxorCoreCacheDelta> tryLock();

	private:
		std::unique_ptr<state::BitxorCoreState> m_pDependentState;
		std::vector<std::unique_ptr<DetachedSubCacheView>> m_detachedSubViews;
	};
}}
