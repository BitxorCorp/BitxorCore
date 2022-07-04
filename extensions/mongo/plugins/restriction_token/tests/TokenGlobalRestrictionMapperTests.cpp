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

#include "src/TokenGlobalRestrictionMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/restriction_token/src/model/TokenGlobalRestrictionTransaction.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "mongo/tests/test/MongoTransactionPluginTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS TokenGlobalRestrictionMapperTests

	namespace {
		DEFINE_MONGO_TRANSACTION_PLUGIN_TEST_TRAITS_NO_ADAPT(TokenGlobalRestriction,)
	}

	DEFINE_BASIC_MONGO_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , model::Entity_Type_Token_Global_Restriction)

	// region streamTransaction

	PLUGIN_TEST(CanMapTokenGlobalRestrictionTransaction) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		transaction.TokenId = test::GenerateRandomValue<UnresolvedTokenId>();
		transaction.ReferenceTokenId = test::GenerateRandomValue<UnresolvedTokenId>();
		transaction.RestrictionKey = test::Random();
		transaction.PreviousRestrictionValue = test::Random();
		transaction.PreviousRestrictionType = static_cast<model::TokenRestrictionType>(test::RandomByte());
		transaction.NewRestrictionValue = test::Random();
		transaction.NewRestrictionType = static_cast<model::TokenRestrictionType>(test::RandomByte());

		auto pPlugin = TTraits::CreatePlugin();

		// Act:
		mappers::bson_stream::document builder;
		pPlugin->streamTransaction(builder, transaction);
		auto view = builder.view();

		// Assert:
		auto previousRestrictionType = static_cast<model::TokenRestrictionType>(test::GetUint8(view, "previousRestrictionType"));
		auto newRestrictionType = static_cast<model::TokenRestrictionType>(test::GetUint8(view, "newRestrictionType"));
		EXPECT_EQ(7u, test::GetFieldCount(view));
		EXPECT_EQ(transaction.TokenId, UnresolvedTokenId(test::GetUint64(view, "tokenId")));
		EXPECT_EQ(transaction.ReferenceTokenId, UnresolvedTokenId(test::GetUint64(view, "referenceTokenId")));
		EXPECT_EQ(transaction.RestrictionKey, test::GetUint64(view, "restrictionKey"));
		EXPECT_EQ(transaction.PreviousRestrictionValue, test::GetUint64(view, "previousRestrictionValue"));
		EXPECT_EQ(transaction.PreviousRestrictionType, previousRestrictionType);
		EXPECT_EQ(transaction.NewRestrictionValue, test::GetUint64(view, "newRestrictionValue"));
		EXPECT_EQ(transaction.NewRestrictionType, newRestrictionType);
	}

	// endregion
}}}
