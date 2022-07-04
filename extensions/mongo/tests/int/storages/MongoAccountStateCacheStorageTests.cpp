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

#include "mongo/src/storages/MongoAccountStateCacheStorage.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "mongo/tests/test/MongoFlatCacheStorageTests.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/AccountStateTestUtils.h"
#include "tests/TestHarness.h"

using namespace bsoncxx::builder::stream;

namespace bitxorcore { namespace mongo { namespace storages {

#define TEST_CLASS MongoAccountStateCacheStorageTests

	namespace {
		constexpr auto Currency_Token_Id = TokenId(1234);

		struct AccountStateCacheTraits {
			using CacheType = cache::AccountStateCache;
			using ModelType = state::AccountState;

			static constexpr auto Collection_Name = "accounts";
			static constexpr auto Primary_Document_Name = "account";
			static constexpr auto Network_Id = static_cast<model::NetworkIdentifier>(0x5A);
			static constexpr auto CreateCacheStorage = CreateMongoAccountStateCacheStorage;

			static cache::BitxorCoreCache CreateCache() {
				auto chainConfig = model::BlockchainConfiguration::Uninitialized();
				chainConfig.Network.Identifier = model::NetworkIdentifier::Testnet;
				return test::CreateEmptyBitxorCoreCache(chainConfig);
			}

			static ModelType GenerateRandomElement(uint32_t id) {
				auto height = Height(id);
				auto publicKey = test::GenerateRandomByteArray<Key>();
				auto address = model::PublicKeyToAddress(publicKey, model::NetworkIdentifier::Testnet);
				auto accountState = state::AccountState(address, Height(1234567) + height);
				accountState.PublicKey = publicKey;
				accountState.PublicKeyHeight = Height(1234567) + height;
				auto randomAmount = Amount((test::Random() % 1'000'000 + 1'000) * 1'000'000);
				auto randomImportance = Importance(test::Random() % 1'000'000'000 + 1'000'000'000);
				auto randomImportanceHeight = test::GenerateRandomValue<model::ImportanceHeight>();
				accountState.Balances.credit(Currency_Token_Id, randomAmount);
				accountState.ImportanceSnapshots.set(randomImportance, randomImportanceHeight);
				return accountState;
			}

			static void Add(cache::BitxorCoreCacheDelta& delta, const ModelType& accountState) {
				auto& accountStateCacheDelta = delta.sub<cache::AccountStateCache>();
				accountStateCacheDelta.addAccount(accountState.PublicKey, accountState.PublicKeyHeight);
				auto& accountStateFromCache = accountStateCacheDelta.find(accountState.PublicKey).get();
				accountStateFromCache.Balances.credit(Currency_Token_Id, accountState.Balances.get(Currency_Token_Id));

				auto height = accountState.ImportanceSnapshots.height();
				accountStateFromCache.ImportanceSnapshots.set(accountState.ImportanceSnapshots.get(height), height);
			}

			static void Remove(cache::BitxorCoreCacheDelta& delta, const ModelType& accountState) {
				auto& accountStateCacheDelta = delta.sub<cache::AccountStateCache>();
				accountStateCacheDelta.queueRemove(accountState.PublicKey, accountState.PublicKeyHeight);
				accountStateCacheDelta.commitRemovals();
			}

			static void Mutate(cache::BitxorCoreCacheDelta& delta, ModelType& accountState) {
				// update expected
				accountState.Balances.credit(Currency_Token_Id, Amount(12'345'000'000));

				// update cache
				auto& accountStateCacheDelta = delta.sub<cache::AccountStateCache>();
				auto& accountStateFromCache = accountStateCacheDelta.find(accountState.PublicKey).get();
				accountStateFromCache.Balances.credit(Currency_Token_Id, Amount(12'345'000'000));
			}

			static auto GetFindFilter(const ModelType& accountState) {
				return document()
						<< std::string(Primary_Document_Name) + ".address" << mappers::ToBinary(accountState.Address)
						<< finalize;
			}

			static void AssertEqual(const ModelType& accountState, const bsoncxx::document::view& view) {
				test::AssertEqualAccountState(accountState, view[Primary_Document_Name].get_document().view());
			}
		};
	}

	DEFINE_FLAT_CACHE_STORAGE_TESTS(AccountStateCacheTraits,)
}}}
