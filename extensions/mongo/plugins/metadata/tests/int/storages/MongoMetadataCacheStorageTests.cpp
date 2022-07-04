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

#include "src/storages/MongoMetadataCacheStorage.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "mongo/tests/test/MongoFlatCacheStorageTests.h"
#include "mongo/tests/test/MongoTestUtils.h"
#include "plugins/txes/metadata/tests/test/MetadataCacheTestUtils.h"
#include "plugins/txes/metadata/tests/test/MetadataTestUtils.h"
#include "tests/test/MetadataMapperTestUtils.h"
#include "tests/TestHarness.h"

using namespace bsoncxx::builder::stream;

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS MongoMetadataCacheStorageTests

	namespace {
		struct BasicMetadataCacheTraits {
		public:
			using CacheType = cache::MetadataCache;
			using ModelType = state::MetadataEntry;

			static constexpr auto Collection_Name = "metadata";
			static constexpr auto Primary_Document_Name = "metadataEntry";
			static constexpr auto Network_Id = static_cast<model::NetworkIdentifier>(0x5A);
			static constexpr auto CreateCacheStorage = CreateMongoMetadataCacheStorage;

		public:
			static cache::BitxorCoreCache CreateCache() {
				return test::MetadataCacheFactory::Create();
			}

			static void Add(cache::BitxorCoreCacheDelta& delta, const ModelType& metadataEntry) {
				auto& metadataCacheDelta = delta.sub<cache::MetadataCache>();
				metadataCacheDelta.insert(metadataEntry);
			}

			static void Remove(cache::BitxorCoreCacheDelta& delta, const ModelType& metadataEntry) {
				auto& metadataCacheDelta = delta.sub<cache::MetadataCache>();
				metadataCacheDelta.remove(metadataEntry.key().uniqueKey());
			}

			static void Mutate(cache::BitxorCoreCacheDelta& delta, ModelType& metadataEntry) {
				// update expected
				auto valueBuffer = test::GenerateRandomVector(5);
				metadataEntry.value().update(valueBuffer);

				// update cache
				auto& metadataCacheDelta = delta.sub<cache::MetadataCache>();
				auto& metadataEntryFromCache = metadataCacheDelta.find(metadataEntry.key().uniqueKey()).get();
				metadataEntryFromCache.value().update(valueBuffer);
			}

			static auto GetFindFilter(const ModelType& metadataEntry) {
				return document()
						<< std::string(Primary_Document_Name) + ".compositeHash" << mappers::ToBinary(metadataEntry.key().uniqueKey())
						<< finalize;
			}

			static void AssertEqual(const ModelType& metadataEntry, const bsoncxx::document::view& view) {
				test::AssertEqualMetadataEntry(metadataEntry, view[Primary_Document_Name].get_document().view());
			}
		};
	}

	struct AccountMetadataCacheTraits : public BasicMetadataCacheTraits {
	public:
		static ModelType GenerateRandomElement(uint32_t id) {
			return ModelType(state::MetadataKey(test::GenerateRandomPartialMetadataKey(id)));
		}
	};

	struct TokenMetadataCacheTraits : public BasicMetadataCacheTraits {
	public:
		static ModelType GenerateRandomElement(uint32_t id) {
			return ModelType(state::MetadataKey(test::GenerateRandomPartialMetadataKey(id), TokenId(id)));
		}
	};

	struct NamespaceMetadataCacheTraits : public BasicMetadataCacheTraits {
	public:
		static ModelType GenerateRandomElement(uint32_t id) {
			return ModelType(state::MetadataKey(test::GenerateRandomPartialMetadataKey(id), NamespaceId(id)));
		}
	};

	DEFINE_FLAT_CACHE_STORAGE_TESTS(AccountMetadataCacheTraits, _Account)
	DEFINE_FLAT_CACHE_STORAGE_TESTS(TokenMetadataCacheTraits, _Token)
	DEFINE_FLAT_CACHE_STORAGE_TESTS(NamespaceMetadataCacheTraits, _Namespace)
}}}
