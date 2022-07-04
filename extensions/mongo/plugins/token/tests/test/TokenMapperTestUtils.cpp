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

#include "TokenMapperTestUtils.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/token/src/state/TokenEntry.h"
#include "bitxorcore/utils/Casting.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	namespace {
		void AssertTokenProperties(const model::TokenProperties& properties, const bsoncxx::document::view& dbProperties) {
			EXPECT_EQ(properties.flags(), static_cast<model::TokenFlags>(GetUint8(dbProperties, "flags")));
			EXPECT_EQ(properties.divisibility(), GetUint8(dbProperties, "divisibility"));
			EXPECT_EQ(properties.duration(), BlockDuration(GetUint64(dbProperties, "duration")));
		}
	}

	void AssertEqualTokenData(const state::TokenEntry& tokenEntry, const bsoncxx::document::view& dbTokenEntry) {
		EXPECT_EQ(9u, GetFieldCount(dbTokenEntry));
		EXPECT_EQ(1u, GetUint32(dbTokenEntry, "version"));

		EXPECT_EQ(tokenEntry.tokenId(), TokenId(GetUint64(dbTokenEntry, "id")));
		EXPECT_EQ(tokenEntry.supply(), Amount(GetUint64(dbTokenEntry, "supply")));

		const auto& definition = tokenEntry.definition();
		EXPECT_EQ(definition.startHeight(), Height(GetUint64(dbTokenEntry, "startHeight")));
		EXPECT_EQ(definition.ownerAddress(), GetAddressValue(dbTokenEntry, "ownerAddress"));
		EXPECT_EQ(definition.revision(), GetUint32(dbTokenEntry, "revision"));

		AssertTokenProperties(definition.properties(), dbTokenEntry);
	}
}}
