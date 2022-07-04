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

#include "src/mappers/MetadataEntryMapper.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/test/MetadataMapperTestUtils.h"
#include "tests/test/MetadataTestUtils.h"
#include "tests/TestHarness.h"
#include <set>

namespace bitxorcore { namespace mongo { namespace plugins {

#define TEST_CLASS MetadataEntryMapperTests

	// region ToDbModel

	namespace {
		template<typename TTraits>
		void AssertCanMapMetadataEntry(size_t valueSize) {
			// Arrange:
			auto metadataEntry = TTraits::CreateMetadataEntry();
			metadataEntry.value().update(test::GenerateRandomVector(valueSize));

			// Act:
			auto document = ToDbModel(metadataEntry);
			auto view = document.view();

			// Assert:
			EXPECT_EQ(1u, test::GetFieldCount(view));

			auto dbMetadataEntry = view["metadataEntry"].get_document().view();
			test::AssertEqualMetadataEntry(metadataEntry, dbMetadataEntry);
		}

		struct AccountMetadataTraits {
			static state::MetadataEntry CreateMetadataEntry() {
				return state::MetadataEntry(state::MetadataKey(test::GenerateRandomPartialMetadataKey()));
			}
		};

		struct TokenMetadataTraits {
			static state::MetadataEntry CreateMetadataEntry() {
				return state::MetadataEntry(state::MetadataKey(test::GenerateRandomPartialMetadataKey(), TokenId(123)));
			}
		};

		struct NamespaceMetadataTraits {
			static state::MetadataEntry CreateMetadataEntry() {
				return state::MetadataEntry(state::MetadataKey(test::GenerateRandomPartialMetadataKey(), NamespaceId(234)));
			}
		};
	}

#define METADATA_TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Account) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AccountMetadataTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Token) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TokenMetadataTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Namespace) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<NamespaceMetadataTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	METADATA_TRAITS_BASED_TEST(CanMapMetadataEntry_ZeroValueSize) {
		AssertCanMapMetadataEntry<TTraits>(0);
	}

	METADATA_TRAITS_BASED_TEST(CanMapMetadataEntry_NonzeroValueSize) {
		for (auto valueSize : { 1u, 8u, 123u, 257u })
			AssertCanMapMetadataEntry<TTraits>(static_cast<uint16_t>(valueSize));
	}

	// endregion
}}}
