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
#include "src/cache/MultisigCache.h"
#include "src/cache/MultisigCacheStorage.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/cache/CacheTestUtils.h"

namespace bitxorcore { namespace test {

	/// Cache factory for creating a bitxorcore cache composed of multisig cache and core caches.
	struct MultisigCacheFactory {
	private:
		static auto CreateSubCachesWithMultisigCache() {
			auto cacheId = cache::MultisigCache::Id;
			std::vector<std::unique_ptr<cache::SubCachePlugin>> subCaches(cacheId + 1);
			subCaches[cacheId] = MakeSubCachePlugin<cache::MultisigCache, cache::MultisigCacheStorage>();
			return subCaches;
		}

	public:
		/// Creates an empty bitxorcore cache around default configuration.
		static cache::BitxorCoreCache Create() {
			return Create(model::BlockchainConfiguration::Uninitialized());
		}

		/// Creates an empty bitxorcore cache around \a config.
		static cache::BitxorCoreCache Create(const model::BlockchainConfiguration& config) {
			auto subCaches = CreateSubCachesWithMultisigCache();
			CoreSystemCacheFactory::CreateSubCaches(config, subCaches);
			return cache::BitxorCoreCache(std::move(subCaches));
		}
	};
}}
