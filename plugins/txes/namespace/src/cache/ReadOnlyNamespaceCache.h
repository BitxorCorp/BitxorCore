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
#include "src/state/NamespaceEntry.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"

namespace bitxorcore {
	namespace cache {
		class BasicNamespaceCacheDelta;
		class BasicNamespaceCacheView;
	}
}

namespace bitxorcore { namespace cache {

	using ReadOnlyNamespaceArtifactCache = ReadOnlyArtifactCache<
		BasicNamespaceCacheView,
		BasicNamespaceCacheDelta,
		NamespaceId,
		state::NamespaceEntry>;

	/// Read-only overlay on top of a namespace cache.
	class ReadOnlyNamespaceCache : public ReadOnlyNamespaceArtifactCache {
	public:
		/// Creates a read-only overlay on top of \a cache.
		explicit ReadOnlyNamespaceCache(const BasicNamespaceCacheView& cache);

		/// Creates a read-only overlay on top of \a cache.
		explicit ReadOnlyNamespaceCache(const BasicNamespaceCacheDelta& cache);

	public:
		/// Gets the grace period duration.
		BlockDuration gracePeriodDuration() const;

	private:
		const BasicNamespaceCacheView* m_pCache;
		const BasicNamespaceCacheDelta* m_pCacheDelta;
	};
}}
