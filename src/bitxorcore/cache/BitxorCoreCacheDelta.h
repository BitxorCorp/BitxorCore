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
	namespace cache { class ReadOnlyBitxorCoreCache; }
	namespace state { struct BitxorCoreState; }
}

namespace bitxorcore { namespace cache {

	/// Delta on top of a bitxorcore cache.
	class BitxorCoreCacheDelta {
	public:
		/// Disposition of delta.
		enum class Disposition {
			/// Delta can be used to modify the underlying cache.
			Attached,

			/// Delta is detached and cannot be used to modify the underlying cache.
			Detached
		};

	public:
		/// Creates a locked bitxorcore cache delta from \a disposition, \a dependentState and \a subViews.
		BitxorCoreCacheDelta(
				Disposition disposition,
				state::BitxorCoreState& dependentState,
				std::vector<std::unique_ptr<SubCacheView>>&& subViews);

		/// Destroys the delta.
		~BitxorCoreCacheDelta();

	public:
		// make this class move only (the definitions are in the source file in order to allow forward declarations)
		BitxorCoreCacheDelta(BitxorCoreCacheDelta&&);
		BitxorCoreCacheDelta& operator=(BitxorCoreCacheDelta&&);

	public:
		/// Gets a specific sub cache delta view.
		template<typename TCache>
		const typename TCache::CacheDeltaType& sub() const {
			return *static_cast<const typename TCache::CacheDeltaType*>(m_subViews[TCache::Id]->get());
		}

		/// Gets a specific sub cache delta view.
		template<typename TCache>
		typename TCache::CacheDeltaType& sub() {
			return *static_cast<typename TCache::CacheDeltaType*>(m_subViews[TCache::Id]->get());
		}

	public:
		/// Gets the delta disposition.
		Disposition disposition() const;

		/// Gets the (const) dependent bitxorcore state.
		const state::BitxorCoreState& dependentState() const;

		/// Gets the dependent bitxorcore state.
		state::BitxorCoreState& dependentState();

		/// Calculates the cache state hash given \a height.
		StateHashInfo calculateStateHash(Height height) const;

		/// Sets the merkle roots for all sub caches (\a subCacheMerkleRoots).
		void setSubCacheMerkleRoots(const std::vector<Hash256>& subCacheMerkleRoots);

		/// Prunes the cache at \a height.
		void prune(Height height);

		/// Prunes the cache at \a time.
		void prune(Timestamp time);

	public:
		/// Creates a read-only view of this delta.
		ReadOnlyBitxorCoreCache toReadOnly() const;

	private:
		Disposition m_disposition;
		state::BitxorCoreState* m_pDependentState; // use a pointer to allow move assignment
		std::vector<std::unique_ptr<SubCacheView>> m_subViews;
	};
}}
