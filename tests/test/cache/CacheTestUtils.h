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
#include "bitxorcore/cache/CacheConfiguration.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache/SubCachePluginAdapter.h"
#include <memory>
#include <vector>

namespace bitxorcore { namespace cache { class ReadOnlyBitxorCoreCache; } }

namespace bitxorcore { namespace test {

	// region CoreSystemCacheFactory

	/// Cache factory for creating a bitxorcore cache composed of all core sub caches.
	struct CoreSystemCacheFactory {
		/// Creates an empty bitxorcore cache around \a config.
		static cache::BitxorCoreCache Create(const model::BlockchainConfiguration& config);

		/// Adds all core sub caches initialized with \a config to \a subCaches.
		static void CreateSubCaches(
				const model::BlockchainConfiguration& config,
				std::vector<std::unique_ptr<cache::SubCachePlugin>>& subCaches);

		/// Adds all core sub caches initialized with \a config and \a cacheConfig to \a subCaches.
		static void CreateSubCaches(
				const model::BlockchainConfiguration& config,
				const cache::CacheConfiguration& cacheConfig,
				std::vector<std::unique_ptr<cache::SubCachePlugin>>& subCaches);
	};

	// endregion

	// region SubCachePlugin factories

	/// Creates a sub cache plugin given \a args for a plugin that doesn't require configuration.
	template<typename TCache, typename TStorageTraits, typename... TArgs>
	std::unique_ptr<cache::SubCachePlugin> MakeConfigurationFreeSubCachePlugin(TArgs&&... args) {
		auto pCache = std::make_unique<TCache>(std::forward<TArgs>(args)...);
		return std::make_unique<cache::SubCachePluginAdapter<TCache, TStorageTraits>>(std::move(pCache));
	}

	/// Creates a sub cache plugin around \a cacheConfig given \a args.
	template<typename TCache, typename TStorageTraits, typename... TArgs>
	std::unique_ptr<cache::SubCachePlugin> MakeSubCachePluginWithCacheConfiguration(
			const cache::CacheConfiguration& cacheConfig,
			TArgs&&... args) {
		return MakeConfigurationFreeSubCachePlugin<TCache, TStorageTraits>(cacheConfig, std::forward<TArgs>(args)...);
	}

	/// Creates a sub cache plugin given \a args.
	template<typename TCache, typename TStorageTraits, typename... TArgs>
	std::unique_ptr<cache::SubCachePlugin> MakeSubCachePlugin(TArgs&&... args) {
		return MakeSubCachePluginWithCacheConfiguration<TCache, TStorageTraits>(cache::CacheConfiguration(), std::forward<TArgs>(args)...);
	}

	// endregion

	// region CreateEmptyBitxorCoreCache

	/// Creates an empty bitxorcore cache.
	cache::BitxorCoreCache CreateEmptyBitxorCoreCache();

	/// Creates an empty bitxorcore cache around \a config.
	cache::BitxorCoreCache CreateEmptyBitxorCoreCache(const model::BlockchainConfiguration& config);

	/// Creates an empty bitxorcore cache around \a config and \a cacheConfig.
	cache::BitxorCoreCache CreateEmptyBitxorCoreCache(
			const model::BlockchainConfiguration& config,
			const cache::CacheConfiguration& cacheConfig);

	/// Creates an empty bitxorcore cache around \a config.
	template<typename TCacheFactory>
	cache::BitxorCoreCache CreateEmptyBitxorCoreCache(const model::BlockchainConfiguration& config) {
		return TCacheFactory::Create(config);
	}

	// endregion

	// region cache marker utils

	/// Creates a bitxorcore cache with a marker account.
	cache::BitxorCoreCache CreateBitxorCoreCacheWithMarkerAccount();

	/// Creates a bitxorcore cache with a marker account and a specified \a height.
	cache::BitxorCoreCache CreateBitxorCoreCacheWithMarkerAccount(Height height);

	/// Adds a marker account to \a cache.
	void AddMarkerAccount(cache::BitxorCoreCache& cache);

	/// Modes for checking a marked cache.
	enum class IsMarkedCacheMode {
		/// Require the cache to exclusively contain a marked account.
		Exclusive,

		/// Require the cache to contain a marked account but allow other accounts.
		Any
	};

	/// Returns \c true if \a cache contains the marker account and satisfies all \a mode constraints.
	bool IsMarkedCache(const cache::ReadOnlyBitxorCoreCache& cache, IsMarkedCacheMode mode = IsMarkedCacheMode::Exclusive);

	/// Returns \c true if \a cache contains the marker account and satisfies all \a mode constraints.
	bool IsMarkedCache(const cache::BitxorCoreCacheDelta& cache, IsMarkedCacheMode mode = IsMarkedCacheMode::Exclusive);

	// endregion

	// region ExtractValuesFromCache

	/// Extract all values from \a cache.
	template<typename TCache, typename TValue>
	std::vector<TValue> ExtractValuesFromCache(const TCache& cache) {
		std::vector<TValue> values;
		auto view = cache.createView();
		for (const auto& pair : *view)
			values.push_back(pair.second);

		return values;
	}

	// endregion
}}
