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

#include "bitxorcore/validators/ValidatorContext.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS ValidatorContextTests

	namespace {
		model::ResolverContext CreateResolverContext() {
			return model::ResolverContext(
					[](const auto& unresolved) { return TokenId(unresolved.unwrap() * 2); },
					[](const auto& unresolved) { return Address{ { unresolved[0] } }; });
		}
	}

	TEST(TEST_CLASS, CanCreateValidatorContextAroundParameters) {
		// Arrange:
		model::NetworkInfo networkInfo;
		networkInfo.Identifier = static_cast<model::NetworkIdentifier>(0xAD);

		auto cache = test::CreateEmptyBitxorCoreCache();
		auto cacheView = cache.createView();
		auto readOnlyCache = cacheView.toReadOnly();

		// Act:
		auto notificationContext = model::NotificationContext(Height(1234), CreateResolverContext());
		auto context = ValidatorContext(notificationContext, Timestamp(987), networkInfo, readOnlyCache);

		// Assert:
		EXPECT_EQ(Timestamp(987), context.BlockTime);
		EXPECT_EQ(static_cast<model::NetworkIdentifier>(0xAD), context.Network.Identifier);
		EXPECT_EQ(&readOnlyCache, &context.Cache);

		EXPECT_EQ(Height(1234), context.Height);

		// - resolvers are copied into context and wired up correctly
		EXPECT_EQ(TokenId(48), context.Resolvers.resolve(UnresolvedTokenId(24)));
		EXPECT_EQ(Address{ { 11 } }, context.Resolvers.resolve(UnresolvedAddress{ { 11, 32 } }));
	}
}}
