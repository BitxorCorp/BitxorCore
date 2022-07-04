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

#include "src/TokenMetadataMapper.h"
#include "plugins/txes/metadata/src/model/TokenMetadataTransaction.h"
#include "mongo/tests/test/MongoTransactionPluginTests.h"
#include "tests/test/MetadataMapperTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS TokenMetadataMapperTests

	namespace {
		DEFINE_MONGO_TRANSACTION_PLUGIN_TEST_TRAITS_NO_ADAPT(TokenMetadata,)
	}

	DEFINE_BASIC_MONGO_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , model::Entity_Type_Token_Metadata)

	// region streamTransaction

	PLUGIN_TEST(CanMapTokenMetadataTransaction_ZeroValueSize) {
		test::AssertCanMapTransaction<TTraits, test::TokenMetadataTestTraits>(0);
	}

	PLUGIN_TEST(CanMapTokenMetadataTransaction_NonzeroValueSize) {
		for (auto valueSize : { 1u, 8u, 123u, 257u })
			test::AssertCanMapTransaction<TTraits, test::TokenMetadataTestTraits>(static_cast<uint16_t>(valueSize));
	}

	// endregion
}}}
