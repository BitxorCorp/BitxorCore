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
#include "StateHashInfo.h"
#include "SubCachePlugin.h"
#include <memory>

namespace bitxorcore {
	namespace cache {
		class CacheHeightView;
		class ReadOnlyBitxorCoreCache;
	}
	namespace state { struct BitxorCoreState; }
}

namespace bitxorcore { namespace cache {

	/// Locked view on top of the bitxorcore cache.
	class BitxorCoreCacheView {
	public:
		/// Creates a locked bitxorcore cache view from a cache height view (\a cacheHeightView), \a dependentState and \a subViews.
		BitxorCoreCacheView(
				CacheHeightView&& cacheHeightView,
				const state::BitxorCoreState& dependentState,
				std::vector<std::unique_ptr<const SubCacheView>>&& subViews);

		/// Releases the read lock.
		~BitxorCoreCacheView();

	public:
		// make this class move only (the definitions are in the source file in order to allow forward declarations)
		BitxorCoreCacheView(BitxorCoreCacheView&&);
		BitxorCoreCacheView& operator=(BitxorCoreCacheView&&);

	public:
		/// Gets a specific sub cache view.
		template<typename TCache>
		const typename TCache::CacheViewType& sub() const {
			return *static_cast<const typename TCache::CacheViewType*>(m_subViews[TCache::Id]->get());
		}

	public:
		/// Gets the cache height associated with the read lock.
		Height height() const;

		/// Gets the (const) dependent bitxorcore state.
		const state::BitxorCoreState& dependentState() const;

		/// Calculates the cache state hash.
		StateHashInfo calculateStateHash() const;

	public:
		/// Creates a read-only view of this view.
		ReadOnlyBitxorCoreCache toReadOnly() const;

	private:
		std::unique_ptr<CacheHeightView> m_pCacheHeight; // use a unique_ptr to allow fwd declare
		const state::BitxorCoreState* m_pDependentState; // use a pointer to allow move assignment
		std::vector<std::unique_ptr<const SubCacheView>> m_subViews;
	};
}}
