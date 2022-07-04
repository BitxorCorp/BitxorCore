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

#include "src/TokenDefinitionMapper.h"
#include "sdk/src/builders/TokenDefinitionBuilder.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "bitxorcore/constants.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "mongo/tests/test/MongoTransactionPluginTests.h"
#include "plugins/txes/token/tests/test/TokenTestUtils.h"
#include "tests/test/core/AddressTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS TokenDefinitionMapperTests

	namespace {
		DEFINE_MONGO_TRANSACTION_PLUGIN_TEST_TRAITS(TokenDefinition)

		auto CreateTokenDefinitionTransactionBuilder(const Key& signer, TokenNonce nonce, const model::TokenProperties& properties) {
			auto networkIdentifier = model::NetworkIdentifier::Testnet;
			builders::TokenDefinitionBuilder builder(networkIdentifier, signer);
			builder.setNonce(nonce);
			builder.setFlags(properties.flags());
			builder.setDivisibility(properties.divisibility());
			builder.setDuration(properties.duration());
			return builder;
		}

		void AssertTokenDefinitionData(
				TokenId id,
				TokenNonce nonce,
				const model::TokenProperties& properties,
				const bsoncxx::document::view& dbTransaction) {
			// Assert:
			EXPECT_EQ(id, TokenId(test::GetUint64(dbTransaction, "id")));
			EXPECT_EQ(nonce, TokenNonce(test::GetUint32(dbTransaction, "nonce")));
			EXPECT_EQ(properties.flags(), static_cast<model::TokenFlags>(test::GetUint8(dbTransaction, "flags")));
			EXPECT_EQ(properties.divisibility(), test::GetUint8(dbTransaction, "divisibility"));
			EXPECT_EQ(properties.duration(), BlockDuration(test::GetUint64(dbTransaction, "duration")));
		}

		template<typename TTraits>
		void AssertCanMapTransaction(BlockDuration duration) {
			auto properties = test::CreateTokenPropertiesFromValues(4, 5, duration.unwrap());
			auto signer = test::GenerateRandomByteArray<Key>();
			auto tokenNonce = test::GenerateRandomValue<TokenNonce>();
			auto pTransaction = TTraits::Adapt(CreateTokenDefinitionTransactionBuilder(signer, tokenNonce, properties));
			auto tokenId = pTransaction->Id;
			auto pPlugin = TTraits::CreatePlugin();

			// Act:
			mappers::bson_stream::document builder;
			pPlugin->streamTransaction(builder, *pTransaction);
			auto view = builder.view();

			// Assert:
			EXPECT_EQ(5u, test::GetFieldCount(view));
			AssertTokenDefinitionData(tokenId, tokenNonce, properties, view);
		}
	}

	DEFINE_BASIC_MONGO_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , model::Entity_Type_Token_Definition)

	// region streamTransaction

	PLUGIN_TEST(CanMapTokenDefinitionTransaction_DefaultDuration) {
		AssertCanMapTransaction<TTraits>(Eternal_Artifact_Duration);
	}

	PLUGIN_TEST(CanMapTokenDefinitionTransaction_CustomDuration) {
		AssertCanMapTransaction<TTraits>(BlockDuration(321));
	}

	// endregion
}}}
