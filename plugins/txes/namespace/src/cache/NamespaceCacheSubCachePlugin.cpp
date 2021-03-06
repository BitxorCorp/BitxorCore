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

#include "NamespaceCacheSubCachePlugin.h"

namespace bitxorcore { namespace cache {

	void NamespaceCacheSummaryCacheStorage::saveAll(const BitxorCoreCacheView&, io::OutputStream&) const {
		BITXORCORE_THROW_INVALID_ARGUMENT("NamespaceCacheSummaryCacheStorage does not support saveAll");
	}

	void NamespaceCacheSummaryCacheStorage::saveSummary(const BitxorCoreCacheDelta& cacheDelta, io::OutputStream& output) const {
		const auto& delta = cacheDelta.sub<NamespaceCache>();
		io::Write64(output, delta.activeSize());
		io::Write64(output, delta.deepSize());
		output.flush();
	}

	void NamespaceCacheSummaryCacheStorage::loadAll(io::InputStream& input, size_t) {
		auto activeSize = io::Read64(input);
		auto deepSize = io::Read64(input);
		cache().init(activeSize, deepSize);
	}

	NamespaceCacheSubCachePlugin::NamespaceCacheSubCachePlugin(
			const CacheConfiguration& config,
			const NamespaceCacheTypes::Options& options)
			: BaseNamespaceCacheSubCachePlugin(std::make_unique<NamespaceCache>(config, options))
	{}
}}
