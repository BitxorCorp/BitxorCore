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

#include "src/state/SecretLockInfo.h"
#include "plugins/txes/lock_shared/tests/state/LockInfoTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS SecretLockInfoTests

	namespace {
		struct SecretLockInfoTraits {
			static void SetLockIdentifier(SecretLockInfo& lockInfo, const Hash256& hash) {
				lockInfo.CompositeHash = hash;
			}
		};
	}

	DEFINE_LOCK_INFO_TESTS(SecretLockInfo)

	TEST(TEST_CLASS, SecretLockInfoConstructorSetsAllFields) {
		// Arrange:
		auto owner = test::GenerateRandomByteArray<Address>();
		auto algorithm = model::LockHashAlgorithm::Op_Hash_160;
		auto secret = test::GenerateRandomByteArray<Hash256>();
		auto recipient = test::GenerateRandomByteArray<Address>();

		// Act:
		SecretLockInfo lockInfo(owner, TokenId(123), Amount(234), Height(345), algorithm, secret, recipient);

		// Assert:
		EXPECT_EQ(owner, lockInfo.OwnerAddress);
		EXPECT_EQ(TokenId(123), lockInfo.TokenId);
		EXPECT_EQ(Amount(234), lockInfo.Amount);
		EXPECT_EQ(Height(345), lockInfo.EndHeight);
		EXPECT_EQ(model::LockHashAlgorithm::Op_Hash_160, lockInfo.HashAlgorithm);
		EXPECT_EQ(secret, lockInfo.Secret);
		EXPECT_EQ(recipient, lockInfo.RecipientAddress);
		EXPECT_EQ(Hash256(), lockInfo.CompositeHash);
	}
}}
