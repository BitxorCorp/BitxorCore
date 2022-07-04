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
#include <vector>

namespace bitxorcore { namespace state { struct BitxorCoreState; } }

namespace bitxorcore { namespace cache {

	/// Read-only overlay on top of a bitxorcore cache.
	class ReadOnlyBitxorCoreCache {
	public:
		/// Creates a read-only overlay on top of \a dependentState and \a readOnlyViews.
		ReadOnlyBitxorCoreCache(const state::BitxorCoreState& dependentState, const std::vector<const void*>& readOnlyViews);

	public:
		/// Gets a specific sub cache read-only view.
		template<typename TCache>
		const typename TCache::CacheReadOnlyType& sub() const {
			return *static_cast<const typename TCache::CacheReadOnlyType*>(m_readOnlyViews[TCache::Id]);
		}

	public:
		/// Gets the (const) dependent bitxorcore state.
		const state::BitxorCoreState& dependentState() const;

	private:
		const state::BitxorCoreState& m_dependentState;
		std::vector<const void*> m_readOnlyViews;
	};
}}
