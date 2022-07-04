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

#include "TokenCacheTestUtils.h"
#include "TokenTestUtils.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	void AddToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, BlockDuration duration, Amount supply) {
		AddToken(cache, id, height, duration, supply, Address());
	}

	void AddToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, BlockDuration duration, const Address& owner) {
		AddToken(cache, id, height, duration, Amount(), owner);
	}

	void AddToken(
			cache::BitxorCoreCacheDelta& cache,
			TokenId id,
			Height height,
			BlockDuration duration,
			Amount supply,
			const Address& owner) {
		auto& tokenCacheDelta = cache.sub<cache::TokenCache>();
		auto definition = state::TokenDefinition(height, owner, 1, test::CreateTokenPropertiesWithDuration(duration));
		auto tokenEntry = state::TokenEntry(id, definition);
		tokenEntry.increaseSupply(supply);
		tokenCacheDelta.insert(tokenEntry);
	}

	void AddEternalToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height) {
		AddEternalToken(cache, id, height, Address());
	}

	void AddEternalToken(cache::BitxorCoreCacheDelta& cache, TokenId id, Height height, const Address& owner) {
		auto& tokenCacheDelta = cache.sub<cache::TokenCache>();
		auto definition = state::TokenDefinition(height, owner, 1, model::TokenProperties());
		tokenCacheDelta.insert(state::TokenEntry(id, definition));
	}

	void AddTokenOwner(cache::BitxorCoreCacheDelta& cache, TokenId id, const Address& owner, Amount amount) {
		auto& accountStateCacheDelta = cache.sub<cache::AccountStateCache>();
		accountStateCacheDelta.addAccount(owner, Height(1));
		accountStateCacheDelta.find(owner).get().Balances.credit(id, amount);
	}

	namespace {
		template<typename TCache>
		void AssertCacheContentsT(const TCache& cache, std::initializer_list<TokenId::ValueType> expectedIds) {
			// Assert:
			EXPECT_EQ(expectedIds.size(), cache.size());
			for (auto id : expectedIds)
				EXPECT_TRUE(cache.contains(TokenId(id))) << "id " << id;
		}
	}

	void AssertCacheContents(const cache::TokenCache& cache, std::initializer_list<TokenId::ValueType> expectedIds) {
		auto view = cache.createView();
		AssertCacheContentsT(*view, expectedIds);
	}

	void AssertCacheContents(const cache::TokenCacheView& cache, std::initializer_list<TokenId::ValueType> expectedIds) {
		AssertCacheContentsT(cache, expectedIds);
	}

	void AssertCacheContents(const cache::TokenCacheDelta& cache, std::initializer_list<TokenId::ValueType> expectedIds) {
		AssertCacheContentsT(cache, expectedIds);
	}
}}
