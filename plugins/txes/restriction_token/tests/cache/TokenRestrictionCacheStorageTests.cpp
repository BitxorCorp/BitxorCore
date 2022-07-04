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

#include "src/cache/TokenRestrictionCacheStorage.h"
#include "src/cache/TokenRestrictionCache.h"
#include "src/model/TokenRestrictionTypes.h"
#include "tests/test/TokenRestrictionTestUtils.h"
#include "tests/test/cache/CacheStorageTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace cache {

	namespace {
		struct TokenRestrictionCacheStorageTraits {
			using StorageType = TokenRestrictionCacheStorage;
			class CacheType : public TokenRestrictionCache {
			public:
				CacheType() : TokenRestrictionCache(CacheConfiguration(), static_cast<model::NetworkIdentifier>(12))
				{}
			};

			static auto CreateId(uint8_t id) {
				return state::CreateTokenRestrictionEntryKey(TokenId(id * id), Address{ { id } });
			}

			static auto CreateValue(const Hash256& hash) {
				state::TokenRestrictionEntry entry(test::GenerateTokenRestrictionEntry(hash));
				auto& restriction = entry.asAddressRestriction();
				for (auto i = 0u; i < 3; ++i)
					restriction.set(i, i * i);

				return entry;
			}

			static void AssertEqual(const state::TokenRestrictionEntry& lhs, const state::TokenRestrictionEntry& rhs) {
				test::AssertEqual(lhs, rhs);
			}
		};
	}

	DEFINE_BASIC_INSERT_REMOVE_CACHE_STORAGE_TESTS(TokenRestrictionCacheStorageTests, TokenRestrictionCacheStorageTraits)
}}
