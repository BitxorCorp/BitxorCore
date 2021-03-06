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

#include "src/extensions/IdGenerator.h"
#include "plugins/txes/token/src/model/TokenIdGenerator.h"
#include "plugins/txes/namespace/src/model/NamespaceIdGenerator.h"
#include "tests/test/nodeps/TestConstants.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS IdGeneratorTests

	// region basic (shared) tests

	namespace {
		template<typename TGenerator>
		void AssertDifferentNamesProduceDifferentResults(TGenerator generator) {
			// Assert:
			for (const auto name : { "bloodyrookie.alice", "cat.nekot", "bloodyrookie.token", "bloody_rookie.token" })
				EXPECT_NE(generator("cat.token"), generator(name)) << "cat.token vs " << name;
		}

		template<typename TGenerator>
		void AssertNamesWithUppercaseCharactersAreRejected(TGenerator generator) {
			// Act + Assert:
			for (const auto name : { "CAT.token", "CAT.TOKEN", "cat.TOKEN", "cAt.ToKeN", "CaT.tOkEn" })
				EXPECT_THROW(generator(name), bitxorcore_invalid_argument) << "name " << name;
		}

		template<typename TGenerator>
		void AssertImproperQualifiedNamesAreRejected(TGenerator generator) {
			// Act + Assert:
			for (const auto name : { ".", "..", "...", ".a", "b.", "a..b", ".a.b", "b.a." })
				EXPECT_THROW(generator(name), bitxorcore_invalid_argument) << "name " << name;
		}

		template<typename TGenerator>
		void AssertImproperPartNamesAreRejected(TGenerator generator) {
			// Act + Assert:
			for (const auto name : { "alpha.bet@.zeta", "a!pha.beta.zeta", "alpha.beta.ze^a" })
				EXPECT_THROW(generator(name), bitxorcore_invalid_argument) << "name " << name;
		}

		template<typename TGenerator>
		void AssertEmptyStringIsRejected(TGenerator generator) {
			// Act + Assert:
			EXPECT_THROW(generator(""), bitxorcore_invalid_argument) << "empty string";
		}
	}

#define ADD_BASIC_TESTS(PREFIX, GENERATOR) \
	TEST(TEST_CLASS, PREFIX##_DifferentNamesProduceDifferentResults) { AssertDifferentNamesProduceDifferentResults(GENERATOR); } \
	TEST(TEST_CLASS, PREFIX##_RejectsNamesWithUppercaseCharacters) { AssertNamesWithUppercaseCharactersAreRejected(GENERATOR); } \
	TEST(TEST_CLASS, PREFIX##_RejectsImproperQualifiedNames) { AssertImproperQualifiedNamesAreRejected(GENERATOR); } \
	TEST(TEST_CLASS, PREFIX##_RejectsImproperPartNames) { AssertImproperPartNamesAreRejected(GENERATOR); } \
	TEST(TEST_CLASS, PREFIX##_RejectsEmptyString) { AssertEmptyStringIsRejected(GENERATOR); }

	// endregion

	// region GenerateTokenAliasId

	TEST(TEST_CLASS, GenerateTokenAliasId_GeneratesCorrectWellKnownId) {
		// Arrange:
		constexpr auto Known_Token_Alias_Id = UnresolvedTokenId(0xA029'E100'621B'2E33ULL);

		// Assert:
		EXPECT_EQ(Known_Token_Alias_Id, GenerateTokenAliasId("cat.token"));
	}

	TEST(TEST_CLASS, GenerateTokenAliasId_SupportsMultiLevelTokens) {
		// Arrange:
		auto namespaceId = model::GenerateNamespaceId(model::GenerateNamespaceId(model::GenerateRootNamespaceId("foo"), "bar"), "baz");
		auto expected = model::GenerateNamespaceId(namespaceId, "xyz");

		// Assert:
		EXPECT_EQ(UnresolvedTokenId(expected.unwrap()), GenerateTokenAliasId("foo.bar.baz.xyz"));
	}

	TEST(TEST_CLASS, GenerateTokenAliasId_CanGenerateIdFromTopLevelNamespace) {
		for (const auto name : { "cat", "token", "alpha" })
			EXPECT_NE(UnresolvedTokenId(), GenerateTokenAliasId(name)) << "name " << name;
	}

	namespace {
		UnresolvedTokenId GenerateTokenIdAdapter(const RawString& namespaceName) {
			// Act:
			return GenerateTokenAliasId(namespaceName);
		}
	}

	ADD_BASIC_TESTS(GenerateTokenId, GenerateTokenIdAdapter)

	// endregion

	// region GenerateNamespacePath

	TEST(TEST_CLASS, GenerateNamespacePath_GeneratesCorrectWellKnownRootPath) {
		// Act:
		auto path = GenerateNamespacePath("cat");

		// Assert:
		EXPECT_EQ(1u, path.size());
		EXPECT_EQ(NamespaceId(test::Default_Namespace_Id), path[0]);
	}

	TEST(TEST_CLASS, GenerateNamespacePath_GeneratesCorrectWellKnownNamespace) {
		// Arrange:
		constexpr auto Known_Namespace_Id = NamespaceId(0xA029'E100'621B'2E33ULL);

		// Act:
		auto path = GenerateNamespacePath("cat.token");

		// Assert:
		EXPECT_EQ(2u, path.size());
		EXPECT_EQ(NamespaceId(test::Default_Namespace_Id), path[0]);
		EXPECT_EQ(Known_Namespace_Id, path[1]);
	}

	TEST(TEST_CLASS, GenerateNamespacePath_SupportsMultiLevelNamespaces) {
		// Arrange:
		std::vector<NamespaceId> expected;
		expected.push_back(model::GenerateRootNamespaceId("foo"));
		expected.push_back(model::GenerateNamespaceId(expected[0], "bar"));
		expected.push_back(model::GenerateNamespaceId(expected[1], "baz"));
		expected.push_back(model::GenerateNamespaceId(expected[2], "xyz"));

		// Assert:
		EXPECT_EQ(expected, GenerateNamespacePath("foo.bar.baz.xyz"));
	}

	ADD_BASIC_TESTS(GenerateNamespacePath, GenerateNamespacePath)

	// endregion
}}
