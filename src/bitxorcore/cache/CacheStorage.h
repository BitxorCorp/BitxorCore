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
#include "CacheStorageInclude.h"
#include "bitxorcore/plugins.h"
#include <string>

namespace bitxorcore {
	namespace cache {
		class BitxorCoreCacheDelta;
		class BitxorCoreCacheView;
	}
}

namespace bitxorcore { namespace cache {

	/// Interface for loading and saving cache data.
	class PLUGIN_API_DEPENDENCY CacheStorage {
	public:
		virtual ~CacheStorage() = default;

	public:
		/// Gets the cache name.
		virtual const std::string& name() const = 0;

	public:
		/// Saves cache data from \a cacheView to \a output.
		virtual void saveAll(const BitxorCoreCacheView& cacheView, io::OutputStream& output) const = 0;

		/// Saves cache (summary) data from \a cacheDelta to \a output.
		virtual void saveSummary(const BitxorCoreCacheDelta& cacheDelta, io::OutputStream& output) const = 0;

		/// Loads cache data from \a input in batches of \a batchSize.
		virtual void loadAll(io::InputStream& input, size_t batchSize) = 0;
	};
}}
