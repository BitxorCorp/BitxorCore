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

#include "src/extensions/ConversionExtensions.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS ConversionExtensionTests

	TEST(TEST_CLASS, CanCastTokenIdToUnresolvedTokenId) {
		// Arrange:
		auto tokenId = TokenId(248);

		// Act:
		auto unresolvedTokenId = CastToUnresolvedTokenId(tokenId);

		// Assert:
		EXPECT_EQ(UnresolvedTokenId(248), unresolvedTokenId);
	}

	TEST(TEST_CLASS, CanCastUnresolvedTokenIdToTokenId) {
		// Arrange:
		auto unresolvedTokenId = UnresolvedTokenId(248);

		// Act:
		auto tokenId = CastToTokenId(unresolvedTokenId);

		// Assert:
		EXPECT_EQ(TokenId(248), tokenId);
	}

	TEST(TEST_CLASS, CanCopyAddressToUnresolvedAddress) {
		// Arrange:
		auto address = test::GenerateRandomByteArray<Address>();

		// Act:
		auto unresolvedAddress = CopyToUnresolvedAddress(address);

		// Assert:
		EXPECT_EQ_MEMORY(address.data(), unresolvedAddress.data(), address.size());
	}

	TEST(TEST_CLASS, CanCopyUnresolvedAddressToAddress) {
		// Arrange:
		auto unresolvedAddress = CopyToUnresolvedAddress(test::GenerateRandomByteArray<Address>());

		// Act:
		auto address = CopyToAddress(unresolvedAddress);

		// Assert:
		EXPECT_EQ_MEMORY(unresolvedAddress.data(), address.data(), unresolvedAddress.size());
	}
}}
