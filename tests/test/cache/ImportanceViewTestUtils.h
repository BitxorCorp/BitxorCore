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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/cache_core/ImportanceView.h"

namespace bitxorcore { namespace test {

	/// Wrapper for an importance view.
	class ImportanceViewWrapper {
	public:
		/// Creates a wrapper around \a cache.
		explicit ImportanceViewWrapper(const cache::AccountStateCache& cache)
				: m_cacheView(cache.createView())
				, m_readOnlyCache(*m_cacheView)
				, m_view(m_readOnlyCache)
		{}

	public:
		/// Gets a const reference to the underlying importance view.
		const cache::ImportanceView& operator*() {
			return m_view;
		}

		/// Gets a const pointer to the underlying importance view.
		const cache::ImportanceView* operator->() {
			return &m_view;
		}

	private:
		cache::LockedCacheView<cache::AccountStateCacheView> m_cacheView;
		cache::ReadOnlyAccountStateCache m_readOnlyCache;
		cache::ImportanceView m_view;
	};

	/// Creates an importance view wrapper around \a cache.
	inline ImportanceViewWrapper CreateImportanceView(const cache::AccountStateCache& cache) {
		return ImportanceViewWrapper(cache);
	}
}}
