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

#include "TokenRestrictionEntryMapperTestUtils.h"
#include "mongo/src/mappers/MapperInclude.h"
#include "plugins/txes/restriction_token/src/state/TokenRestrictionEntry.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	state::TokenAddressRestriction TokenAddressRestrictionTestTraits::CreateRestriction(size_t numValues) {
		auto tokenId = GenerateRandomValue<TokenId>();
		auto address = GenerateRandomByteArray<Address>();
		state::TokenAddressRestriction addressRestriction(tokenId, address);
		for (auto i = 0u; i < numValues; ++i)
			addressRestriction.set(i, Random());

		return addressRestriction;
	}

	void TokenAddressRestrictionTestTraits::AssertEqualRestriction(
			const state::TokenRestrictionEntry& restrictionEntry,
			const bsoncxx::document::view& dbRestrictionEntry) {
		EXPECT_EQ(6u, GetFieldCount(dbRestrictionEntry));
		EXPECT_EQ(1u, GetUint32(dbRestrictionEntry, "version"));

		const auto& restriction = restrictionEntry.asAddressRestriction();
		auto compositeHash = restrictionEntry.uniqueKey();
		EXPECT_EQ(compositeHash, GetHashValue(dbRestrictionEntry, "compositeHash"));

		auto dbEntryType = static_cast<state::TokenRestrictionEntry::EntryType>(GetUint8(dbRestrictionEntry, "entryType"));
		EXPECT_EQ(state::TokenRestrictionEntry::EntryType::Address, dbEntryType);
		EXPECT_EQ(restriction.tokenId(), TokenId(GetUint64(dbRestrictionEntry, "tokenId")));
		EXPECT_EQ(restriction.address(), GetAddressValue(dbRestrictionEntry, "targetAddress"));

		auto dbRestrictions = dbRestrictionEntry["restrictions"].get_array().value;
		auto keys = restriction.keys();
		ASSERT_EQ(keys.size(), GetFieldCount(dbRestrictions));

		for (const auto& dbRestriction : dbRestrictions) {
			auto dbRestrictionView = dbRestriction.get_document().view();
			auto dbKey = GetUint64(dbRestrictionView, "key");
			EXPECT_CONTAINS(keys, dbKey);

			EXPECT_EQ(restriction.get(dbKey), GetUint64(dbRestrictionView, "value"));
			keys.erase(dbKey);
		}
	}

	state::TokenGlobalRestriction TokenGlobalRestrictionTestTraits::CreateRestriction(size_t numValues) {
		state::TokenGlobalRestriction globalRestriction(GenerateRandomValue<TokenId>());
		for (auto i = 0u; i < numValues; ++i) {
			state::TokenGlobalRestriction::RestrictionRule rule;
			rule.ReferenceTokenId = GenerateRandomValue<TokenId>();
			rule.RestrictionValue = Random();
			rule.RestrictionType = static_cast<model::TokenRestrictionType>(2 * i + 1);
			globalRestriction.set(i, rule);
		}

		return globalRestriction;
	}

	void TokenGlobalRestrictionTestTraits::AssertEqualRestriction(
			const state::TokenRestrictionEntry& restrictionEntry,
			const bsoncxx::document::view& dbRestrictionEntry) {
		EXPECT_EQ(5u, GetFieldCount(dbRestrictionEntry));
		EXPECT_EQ(1u, GetUint32(dbRestrictionEntry, "version"));

		const auto& restriction = restrictionEntry.asGlobalRestriction();
		auto compositeHash = restrictionEntry.uniqueKey();
		EXPECT_EQ(compositeHash, GetHashValue(dbRestrictionEntry, "compositeHash"));

		auto dbEntryType = static_cast<state::TokenRestrictionEntry::EntryType>(GetUint8(dbRestrictionEntry, "entryType"));
		EXPECT_EQ(state::TokenRestrictionEntry::EntryType::Global, dbEntryType);
		EXPECT_EQ(restriction.tokenId(), TokenId(GetUint64(dbRestrictionEntry, "tokenId")));

		auto dbRestrictions = dbRestrictionEntry["restrictions"].get_array().value;
		auto keys = restriction.keys();
		ASSERT_EQ(keys.size(), GetFieldCount(dbRestrictions));

		for (const auto& dbRestriction : dbRestrictions) {
			auto dbRestrictionView = dbRestriction.get_document().view();
			auto dbKey = GetUint64(dbRestrictionView, "key");
			EXPECT_CONTAINS(keys, dbKey);

			auto dbRuleView = dbRestrictionView["restriction"].get_document().view();
			state::TokenGlobalRestriction::RestrictionRule rule;
			bool isSet = restriction.tryGet(dbKey, rule);
			ASSERT_TRUE(isSet);

			auto restrictionType = static_cast<model::TokenRestrictionType>(GetUint8(dbRuleView, "restrictionType"));
			EXPECT_EQ(rule.ReferenceTokenId, TokenId(GetUint64(dbRuleView, "referenceTokenId")));
			EXPECT_EQ(rule.RestrictionValue, GetUint64(dbRuleView, "restrictionValue"));
			EXPECT_EQ(rule.RestrictionType, restrictionType);
		}
	}
}}
