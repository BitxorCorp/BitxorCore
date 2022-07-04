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

#include "src/TokenSupplyChangeMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/token/src/model/TokenSupplyChangeTransaction.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "mongo/tests/test/MongoTransactionPluginTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS TokenSupplyChangeMapperTests

	namespace {
		DEFINE_MONGO_TRANSACTION_PLUGIN_TEST_TRAITS_NO_ADAPT(TokenSupplyChange,)
	}

	DEFINE_BASIC_MONGO_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , model::Entity_Type_Token_Supply_Change)

	// region streamTransaction

	PLUGIN_TEST(CanMapSupplyChangeTransaction) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		transaction.TokenId = UnresolvedTokenId(753);
		transaction.Action = model::TokenSupplyChangeAction::Increase;
		transaction.Delta = Amount(12349876);

		auto pPlugin = TTraits::CreatePlugin();

		// Act:
		mappers::bson_stream::document builder;
		pPlugin->streamTransaction(builder, transaction);
		auto view = builder.view();

		// Assert:
		EXPECT_EQ(3u, test::GetFieldCount(view));
		EXPECT_EQ(753u, test::GetUint64(view, "tokenId"));
		EXPECT_EQ(
				model::TokenSupplyChangeAction::Increase,
				static_cast<model::TokenSupplyChangeAction>(test::GetUint32(view, "action")));
		EXPECT_EQ(12349876u, test::GetUint64(view, "delta"));
	}

	// endregion
}}}
