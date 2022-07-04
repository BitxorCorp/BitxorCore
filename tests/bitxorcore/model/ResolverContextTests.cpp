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

#include "bitxorcore/model/ResolverContext.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS ResolverContextTests

	TEST(TEST_CLASS, CanResolveToken_DefaultResolver) {
		// Arrange:
		ResolverContext context;

		// Act:
		auto result = context.resolve(UnresolvedTokenId(123));

		// Assert:
		EXPECT_EQ(TokenId(123), result);
	}

	TEST(TEST_CLASS, CanResolveAddress_DefaultResolver) {
		// Arrange:
		ResolverContext context;

		// Act:
		auto result = context.resolve(UnresolvedAddress{ { { 123 } }});

		// Assert:
		EXPECT_EQ(Address{ { 123 } }, result);
	}

	TEST(TEST_CLASS, CanResolveToken_CustomResolver) {
		// Arrange:
		ResolverContext context(
				[](auto tokenId) { return TokenId(tokenId.unwrap() + 1); },
				[](const auto&) { return Address(); });

		// Act:
		auto result = context.resolve(UnresolvedTokenId(123));

		// Assert:
		EXPECT_EQ(TokenId(124), result);
	}

	TEST(TEST_CLASS, CanResolveAddress_CustomResolver) {
		// Arrange:
		ResolverContext context(
				[](const auto&) { return TokenId(); },
				[](const auto& address) { return Address{ { static_cast<uint8_t>(address[0] + 1) } }; });

		// Act:
		auto result = context.resolve(UnresolvedAddress{ { { 123 } } });

		// Assert:
		EXPECT_EQ(Address{ { 124 } }, result);
	}
}}
