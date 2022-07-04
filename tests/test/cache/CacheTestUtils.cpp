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

#include "CacheTestUtils.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache/SubCachePluginAdapter.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/cache_core/AccountStateCacheStorage.h"
#include "bitxorcore/cache_core/BlockStatisticCacheStorage.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/nodeps/Random.h"

namespace bitxorcore { namespace test {

	namespace {
		Key GetSentinelCachePublicKey() {
			return { { 0xFF, 0xFF, 0xFF, 0xFF } };
		}

		cache::AccountStateCacheTypes::Options CreateAccountStateCacheOptions(const model::BlockchainConfiguration& config) {
			return {
				config.Network.Identifier,
				config.ImportanceGrouping,
				config.VotingSetGrouping,
				config.MinHarvesterBalance,
				config.MaxHarvesterBalance,
				config.MinVoterBalance,
				config.CurrencyTokenId,
				config.HarvestingTokenId
			};
		}
	}

	// region CoreSystemCacheFactory

	cache::BitxorCoreCache CoreSystemCacheFactory::Create(const model::BlockchainConfiguration& config) {
		std::vector<std::unique_ptr<cache::SubCachePlugin>> subCaches(2);
		CreateSubCaches(config, subCaches);
		return cache::BitxorCoreCache(std::move(subCaches));
	}

	void CoreSystemCacheFactory::CreateSubCaches(
			const model::BlockchainConfiguration& config,
			std::vector<std::unique_ptr<cache::SubCachePlugin>>& subCaches) {
		CreateSubCaches(config, cache::CacheConfiguration(), subCaches);
	}

	void CoreSystemCacheFactory::CreateSubCaches(
			const model::BlockchainConfiguration& config,
			const cache::CacheConfiguration& cacheConfig,
			std::vector<std::unique_ptr<cache::SubCachePlugin>>& subCaches) {
		using namespace cache;

		subCaches[AccountStateCache::Id] = MakeSubCachePluginWithCacheConfiguration<AccountStateCache, AccountStateCacheStorage>(
				cacheConfig,
				CreateAccountStateCacheOptions(config));

		subCaches[BlockStatisticCache::Id] = MakeConfigurationFreeSubCachePlugin<BlockStatisticCache, BlockStatisticCacheStorage>(
				config.MaxDifficultyBlocks);
	}

	// endregion

	// region CreateEmptyBitxorCoreCache

	cache::BitxorCoreCache CreateEmptyBitxorCoreCache() {
		auto config = model::BlockchainConfiguration::Uninitialized();
		config.VotingSetGrouping = 1;
		return CreateEmptyBitxorCoreCache(config);
	}

	cache::BitxorCoreCache CreateEmptyBitxorCoreCache(const model::BlockchainConfiguration& config) {
		return CreateEmptyBitxorCoreCache<CoreSystemCacheFactory>(config);
	}

	cache::BitxorCoreCache CreateEmptyBitxorCoreCache(
			const model::BlockchainConfiguration& config,
			const cache::CacheConfiguration& cacheConfig) {
		std::vector<std::unique_ptr<cache::SubCachePlugin>> subCaches(2);
		CoreSystemCacheFactory::CreateSubCaches(config, cacheConfig, subCaches);
		return cache::BitxorCoreCache(std::move(subCaches));
	}

	// endregion

	// region cache marker utils

	cache::BitxorCoreCache CreateBitxorCoreCacheWithMarkerAccount() {
		return CreateBitxorCoreCacheWithMarkerAccount(Height(0));
	}

	cache::BitxorCoreCache CreateBitxorCoreCacheWithMarkerAccount(Height height) {
		auto cache = CreateEmptyBitxorCoreCache();
		AddMarkerAccount(cache);

		auto delta = cache.createDelta();
		cache.commit(height);
		return cache;
	}

	void AddMarkerAccount(cache::BitxorCoreCache& cache) {
		auto delta = cache.createDelta();
		delta.sub<cache::AccountStateCache>().addAccount(GetSentinelCachePublicKey(), Height(1));
		cache.commit(Height(1));
	}

	namespace {
		template<typename TCache>
		bool IsMarkedCacheT(TCache& cache, IsMarkedCacheMode mode) {
			const auto& accountStateCache = cache.template sub<cache::AccountStateCache>();
			if (IsMarkedCacheMode::Exclusive == mode && 1 != accountStateCache.size())
				return false;

			return accountStateCache.contains(GetSentinelCachePublicKey());
		}
	}

	bool IsMarkedCache(const cache::ReadOnlyBitxorCoreCache& cache, IsMarkedCacheMode mode) {
		return IsMarkedCacheT(cache, mode);
	}

	bool IsMarkedCache(const cache::BitxorCoreCacheDelta& cache, IsMarkedCacheMode mode) {
		return IsMarkedCacheT(cache, mode);
	}

	// endregion
}}
