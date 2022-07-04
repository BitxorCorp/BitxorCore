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

#include "BlockStatisticCacheSubCachePlugin.h"

namespace bitxorcore { namespace cache {

	namespace {
		class BlockStatisticCacheSummaryCacheStorage : public CacheStorageAdapter<BlockStatisticCache, BlockStatisticCacheStorage> {
		public:
			using CacheStorageAdapter<BlockStatisticCache, BlockStatisticCacheStorage>::CacheStorageAdapter;

		public:
			void saveSummary(const BitxorCoreCacheDelta& cacheDelta, io::OutputStream& output) const override {
				const auto& delta = cacheDelta.sub<BlockStatisticCache>();
				io::Write64(output, delta.size());

				auto pIterableView = delta.tryMakeIterableView();
				for (const auto& value : *pIterableView)
					BlockStatisticCacheStorage::Save(value, output);

				output.flush();
			}
		};
	}

	BlockStatisticCacheSubCachePlugin::BlockStatisticCacheSubCachePlugin(uint64_t historySize)
			: SubCachePluginAdapter<BlockStatisticCache, BlockStatisticCacheStorage>(std::make_unique<BlockStatisticCache>(historySize))
	{}

	std::unique_ptr<CacheStorage> BlockStatisticCacheSubCachePlugin::createStorage() {
		return std::make_unique<BlockStatisticCacheSummaryCacheStorage>(cache());
	}
}}
