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
#include "AccountRestrictionTestTraits.h"
#include "src/cache/AccountRestrictionCache.h"
#include "src/cache/AccountRestrictionCacheStorage.h"
#include "src/state/AccountRestrictionUtils.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/cache/CacheTestUtils.h"

namespace bitxorcore { namespace test {

	/// Cache factory for creating a bitxorcore cache composed of account restriction cache and core caches.
	struct AccountRestrictionCacheFactory {
	private:
		static auto CreateSubCachesWithAccountRestrictionCache(model::NetworkIdentifier networkIdentifier) {
			auto cacheId = cache::AccountRestrictionCache::Id;
			std::vector<std::unique_ptr<cache::SubCachePlugin>> subCaches(cacheId + 1);
			subCaches[cacheId] = MakeSubCachePlugin<cache::AccountRestrictionCache, cache::AccountRestrictionCacheStorage>(
					networkIdentifier);
			return subCaches;
		}

	public:
		/// Creates an empty bitxorcore cache around default configuration.
		static cache::BitxorCoreCache Create() {
			return Create(model::BlockchainConfiguration::Uninitialized());
		}

		/// Creates an empty bitxorcore cache around \a config.
		static cache::BitxorCoreCache Create(const model::BlockchainConfiguration& config) {
			auto subCaches = CreateSubCachesWithAccountRestrictionCache(config.Network.Identifier);
			CoreSystemCacheFactory::CreateSubCaches(config, subCaches);
			return cache::BitxorCoreCache(std::move(subCaches));
		}
	};

	// region PopulateCache

	/// Populates \a delta with \a address and \a values.
	template<typename TRestrictionValueTraits, typename TOperationTraits = AllowTraits>
	void PopulateCache(
			cache::BitxorCoreCacheDelta& delta,
			const Address& address,
			const std::vector<typename TRestrictionValueTraits::ValueType>& values) {
		auto& restrictionCacheDelta = delta.sub<cache::AccountRestrictionCache>();
		restrictionCacheDelta.insert(state::AccountRestrictions(address));
		auto& restrictions = restrictionCacheDelta.find(address).get();
		auto& restriction = restrictions.restriction(TRestrictionValueTraits::Restriction_Flags);
		for (const auto& value : values)
			TOperationTraits::Add(restriction, state::ToVector(value));
	}

	/// Populates \a cache with \a address and \a values.
	template<typename TRestrictionValueTraits, typename TOperationTraits = AllowTraits>
	void PopulateCache(
			cache::BitxorCoreCache& cache,
			const Address& address,
			const std::vector<typename TRestrictionValueTraits::ValueType>& values) {
		auto delta = cache.createDelta();
		PopulateCache<TRestrictionValueTraits, TOperationTraits>(delta, address, values);
		cache.commit(Height(1));
	}

	// endregion
}}
