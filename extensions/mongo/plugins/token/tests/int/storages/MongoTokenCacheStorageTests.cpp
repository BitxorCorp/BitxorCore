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

#include "src/storages/MongoTokenCacheStorage.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "mongo/tests/test/MongoFlatCacheStorageTests.h"
#include "mongo/tests/test/MongoTestUtils.h"
#include "plugins/txes/token/tests/test/TokenCacheTestUtils.h"
#include "plugins/txes/token/tests/test/TokenTestUtils.h"
#include "tests/test/TokenMapperTestUtils.h"
#include "tests/TestHarness.h"

using namespace bsoncxx::builder::stream;

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS MongoTokenCacheStorageTests

	namespace {
		struct TokenCacheTraits {
			using CacheType = cache::TokenCache;
			using ModelType = state::TokenEntry;

			static constexpr auto Collection_Name = "tokens";
			static constexpr auto Primary_Document_Name = "token";
			static constexpr auto Network_Id = static_cast<model::NetworkIdentifier>(0x5A);
			static constexpr auto CreateCacheStorage = CreateMongoTokenCacheStorage;

			static cache::BitxorCoreCache CreateCache() {
				return test::TokenCacheFactory::Create();
			}

			static ModelType GenerateRandomElement(uint32_t id) {
				auto owner = test::CreateRandomOwner();
				return test::CreateTokenEntry(TokenId(id), Height(345), owner, Amount(456), BlockDuration(567));
			}

			static void Add(cache::BitxorCoreCacheDelta& delta, const ModelType& tokenEntry) {
				auto& tokenCacheDelta = delta.sub<cache::TokenCache>();
				tokenCacheDelta.insert(tokenEntry);
			}

			static void Remove(cache::BitxorCoreCacheDelta& delta, const ModelType& tokenEntry) {
				auto& tokenCacheDelta = delta.sub<cache::TokenCache>();
				tokenCacheDelta.remove(tokenEntry.tokenId());
			}

			static void Mutate(cache::BitxorCoreCacheDelta& delta, ModelType& tokenEntry) {
				// update expected
				tokenEntry.increaseSupply(Amount(1));

				// update cache
				auto& tokenCacheDelta = delta.sub<cache::TokenCache>();
				auto& tokenEntryFromCache = tokenCacheDelta.find(tokenEntry.tokenId()).get();
				tokenEntryFromCache.increaseSupply(Amount(1));
			}

			static auto GetFindFilter(const ModelType& tokenEntry) {
				return document() << std::string(Primary_Document_Name) + ".id" << mappers::ToInt64(tokenEntry.tokenId()) << finalize;
			}

			static void AssertEqual(const ModelType& tokenEntry, const bsoncxx::document::view& view) {
				test::AssertEqualTokenData(tokenEntry, view[Primary_Document_Name].get_document().view());
			}
		};
	}

	DEFINE_FLAT_CACHE_STORAGE_TESTS(TokenCacheTraits,)
}}}
