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
#include "src/cache/TokenCache.h"
#include "src/cache/TokenCacheStorage.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/cache/CacheTestUtils.h"

namespace bitxorcore { namespace test {

	/// Cache factory for creating a bitxorcore cache containing at least the token cache.
	struct TokenCacheFactory {
	private:
		static auto CreateSubCachesWithTokenCache() {
			auto cacheId = cache::TokenCache::Id;
			std::vector<std::unique_ptr<cache::SubCachePlugin>> subCaches(cacheId + 1);
			subCaches[cacheId] = MakeSubCachePlugin<cache::TokenCache, cache::TokenCacheStorage>();
			return subCaches;
		}

	public:
		/// Creates an empty bitxorcore cache.
		static cache::BitxorCoreCache Create() {
			return cache::BitxorCoreCache(CreateSubCachesWithTokenCache());
		}

		/// Creates an empty bitxorcore cache around \a config.
		static cache::BitxorCoreCache Create(const model::BlockchainConfiguration& config) {
			auto subCaches = CreateSubCachesWithTokenCache();
			CoreSystemCacheFactory::CreateSubCaches(config, subCaches);
			return cache::BitxorCoreCache(std::move(subCaches));
		}
	};

	/// Adds token \a id to the token sub cache in \a cache at \a height with \a duration and \a supply.
	void AddToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, BlockDuration duration, Amount supply);

	/// Adds token \a id to the token sub cache in \a cache at \a height with \a duration and \a owner.
	void AddToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, BlockDuration duration, const Address& owner);

	/// Adds token \a id to the token sub cache in \a cache at \a height with \a duration, \a supply and \a owner.
	void AddToken(
			cache::BitxorCoreCacheDelta& cache,
			TokenId id,
			Height height,
			BlockDuration duration,
			Amount supply,
			const Address& owner);

	/// Adds eternal token \a id to the token sub cache in \a cache at \a height.
	void AddEternalToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height);

	/// Adds eternal token \a id to the token sub cache in \a cache at \a height with \a owner.
	void AddEternalToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, const Address& owner);

	/// Adds account \a owner to the account state sub cache in \a cache with \a amount units of token \a id.
	void AddTokenOwner(cache::BitxorCoreCacheDelta& cache, TokenId id, const Address& owner, Amount amount);

	/// Asserts that \a cache exactly contains the token ids in \a expectedIds.
	void AssertCacheContents(const cache::TokenCache& cache, std::initializer_list<TokenId::ValueType> expectedIds);

	/// Asserts that \a cache exactly contains the token ids in \a expectedIds.
	void AssertCacheContents(const cache::TokenCacheView& cache, std::initializer_list<TokenId::ValueType> expectedIds);

	/// Asserts that \a cache exactly contains the token ids in \a expectedIds.
	void AssertCacheContents(const cache::TokenCacheDelta& cache, std::initializer_list<TokenId::ValueType> expectedIds);
}}
