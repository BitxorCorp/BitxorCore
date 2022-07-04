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

#include "src/state/NamespaceAlias.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS NamespaceAliasTests

	namespace {
		void AssertTokenAlias(const NamespaceAlias& alias, TokenId tokenId) {
			// Assert:
			EXPECT_EQ(AliasType::Token, alias.type());
			EXPECT_EQ(tokenId, alias.tokenId());
			EXPECT_THROW(alias.address(), bitxorcore_runtime_error);
		}

		void AssertAddressAlias(const NamespaceAlias& alias, const Address& address) {
			// Assert:
			EXPECT_EQ(AliasType::Address, alias.type());
			EXPECT_THROW(alias.tokenId(), bitxorcore_runtime_error);
			EXPECT_EQ(address, alias.address());
		}
	}

	// region constructor

	TEST(TEST_CLASS, CanCreateUnsetNamespaceAlias) {
		// Act:
		NamespaceAlias alias;

		// Assert:
		EXPECT_EQ(AliasType::None, alias.type());
		EXPECT_THROW(alias.tokenId(), bitxorcore_runtime_error);
		EXPECT_THROW(alias.address(), bitxorcore_runtime_error);
	}

	TEST(TEST_CLASS, CanCreateTokenNamespaceAlias) {
		// Act:
		NamespaceAlias alias(TokenId(245));

		// Assert:
		AssertTokenAlias(alias, TokenId(245));
	}

	TEST(TEST_CLASS, CanCreateAddressNamespaceAlias) {
		// Act:
		auto address = test::GenerateRandomByteArray<Address>();
		NamespaceAlias alias(address);

		// Assert:
		AssertAddressAlias(alias, address);
	}

	// endregion

	// region copy constructor

	TEST(TEST_CLASS, CanCopyTokenNamespaceAlias) {
		// Arrange:
		NamespaceAlias originalAlias(TokenId(245));

		// Act:
		NamespaceAlias alias(originalAlias);

		// Assert:
		AssertTokenAlias(alias, TokenId(245));
	}

	TEST(TEST_CLASS, CanCopyAddressNamespaceAlias) {
		// Arrange:
		auto address = test::GenerateRandomByteArray<Address>();
		NamespaceAlias originalAlias(address);

		// Act:
		NamespaceAlias alias(originalAlias);

		// Assert:
		AssertAddressAlias(alias, address);
	}

	// endregion

	// region assignment operator

	TEST(TEST_CLASS, CanAssignTokenNamespaceAlias) {
		// Arrange:
		NamespaceAlias originalAlias(TokenId(245));

		// Act:
		NamespaceAlias alias;
		const auto& result = (alias = originalAlias);

		// Assert:
		EXPECT_EQ(&alias, &result);
		AssertTokenAlias(alias, TokenId(245));
	}

	TEST(TEST_CLASS, CanAssignAddressNamespaceAlias) {
		// Arrange:
		auto address = test::GenerateRandomByteArray<Address>();
		NamespaceAlias originalAlias(address);

		// Act:
		NamespaceAlias alias;
		const auto& result = (alias = originalAlias);

		// Assert:
		EXPECT_EQ(&alias, &result);
		AssertAddressAlias(alias, address);
	}

	// endregion
}}
