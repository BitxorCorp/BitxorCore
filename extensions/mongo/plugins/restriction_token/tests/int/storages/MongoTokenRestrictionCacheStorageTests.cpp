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

#include "src/storages/MongoTokenRestrictionCacheStorage.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/restriction_token/src/cache/TokenRestrictionCache.h"
#include "mongo/tests/test/MongoFlatCacheStorageTests.h"
#include "mongo/tests/test/MongoTestUtils.h"
#include "plugins/txes/restriction_token/tests/test/TokenRestrictionCacheTestUtils.h"
#include "tests/test/TokenRestrictionEntryMapperTestUtils.h"
#include "tests/TestHarness.h"

using namespace bsoncxx::builder::stream;

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS MongoTokenRestrictionCacheStorageTests

	namespace {
		struct BasicTokenRestrictionCacheTraits {
		public:
			using CacheType = cache::TokenRestrictionCache;
			using ModelType = state::TokenRestrictionEntry;

			static constexpr auto Collection_Name = "tokenRestrictions";
			static constexpr auto Primary_Document_Name = "tokenRestrictionEntry";
			static constexpr auto Network_Id = static_cast<model::NetworkIdentifier>(0x5A);
			static constexpr auto CreateCacheStorage = CreateMongoTokenRestrictionCacheStorage;

		public:
			static cache::BitxorCoreCache CreateCache() {
				return test::TokenRestrictionCacheFactory::Create();
			}

			static void Add(cache::BitxorCoreCacheDelta& delta, const ModelType& restrictionEntry) {
				auto& restrictionCacheDelta = delta.sub<cache::TokenRestrictionCache>();
				restrictionCacheDelta.insert(restrictionEntry);
			}

			static void Remove(cache::BitxorCoreCacheDelta& delta, const ModelType& restrictionEntry) {
				auto& restrictionCacheDelta = delta.sub<cache::TokenRestrictionCache>();
				restrictionCacheDelta.remove(restrictionEntry.uniqueKey());
			}

			static auto GetFindFilter(const ModelType& restrictionEntry) {
				return document()
						<< std::string(Primary_Document_Name) + ".compositeHash" << mappers::ToBinary(restrictionEntry.uniqueKey())
						<< finalize;
			}
		};

		struct TokenAddressRestrictionCacheTraits : public BasicTokenRestrictionCacheTraits {
		public:
			static ModelType GenerateRandomElement(uint32_t id) {
				TokenId tokenId(id);
				Address address;
				std::memcpy(address.data(), &id, sizeof(id));

				auto restrictionEntry = state::TokenRestrictionEntry(state::TokenAddressRestriction(tokenId, address));
				restrictionEntry.asAddressRestriction().set(test::Random(), test::Random());
				return restrictionEntry;
			}

			static void Mutate(cache::BitxorCoreCacheDelta& delta, ModelType& restrictionEntry) {
				// update expected
				auto key = test::Random();
				auto value = test::Random();
				restrictionEntry.asAddressRestriction().set(key, value);

				// update cache
				auto& restrictionCacheDelta = delta.sub<cache::TokenRestrictionCache>();
				auto& restrictionEntryFromCache = restrictionCacheDelta.find(restrictionEntry.uniqueKey()).get();
				restrictionEntryFromCache.asAddressRestriction().set(key, value);
			}

			static void AssertEqual(const ModelType& restrictionEntry, const bsoncxx::document::view& view) {
				auto dbRestrictionEntry = view[Primary_Document_Name].get_document().view();
				test::TokenAddressRestrictionTestTraits::AssertEqualRestriction(restrictionEntry, dbRestrictionEntry);
			}
		};

		struct TokenGlobalRestrictionCacheTraits : public BasicTokenRestrictionCacheTraits {
		public:
			static ModelType GenerateRandomElement(uint32_t id) {
				TokenId tokenId(id);

				auto restrictionEntry = state::TokenRestrictionEntry(state::TokenGlobalRestriction(tokenId));
				restrictionEntry.asGlobalRestriction().set(test::Random(), CreateRandomRestrictionRule());
				return restrictionEntry;
			}

			static void Mutate(cache::BitxorCoreCacheDelta& delta, ModelType& restrictionEntry) {
				// update expected
				auto key = test::Random();
				auto rule = CreateRandomRestrictionRule();
				restrictionEntry.asGlobalRestriction().set(key, rule);

				// update cache
				auto& restrictionCacheDelta = delta.sub<cache::TokenRestrictionCache>();
				auto& restrictionEntryFromCache = restrictionCacheDelta.find(restrictionEntry.uniqueKey()).get();
				restrictionEntryFromCache.asGlobalRestriction().set(key, rule);
			}

			static void AssertEqual(const ModelType& restrictionEntry, const bsoncxx::document::view& view) {
				auto dbRestrictionEntry = view[Primary_Document_Name].get_document().view();
				test::TokenGlobalRestrictionTestTraits::AssertEqualRestriction(restrictionEntry, dbRestrictionEntry);
			}

		private:
			static state::TokenGlobalRestriction::RestrictionRule CreateRandomRestrictionRule() {
				auto referenceTokenId = test::GenerateRandomValue<TokenId>();
				auto restrictionValue = test::Random();
				auto restrictionType = static_cast<model::TokenRestrictionType>(test::RandomByte() | 1);
				return state::TokenGlobalRestriction::RestrictionRule{ referenceTokenId, restrictionValue, restrictionType };
			}
		};
	}

	DEFINE_FLAT_CACHE_STORAGE_TESTS(TokenAddressRestrictionCacheTraits, _Address)
	DEFINE_FLAT_CACHE_STORAGE_TESTS(TokenGlobalRestrictionCacheTraits, _Global)
}}}
