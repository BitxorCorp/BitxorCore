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

#include "SecretLockMapperTestUtils.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/lock_secret/src/state/SecretLockInfo.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace test {

	void AssertEqualLockInfoData(const state::SecretLockInfo& lockInfo, const bsoncxx::document::view& dbLockInfo) {
		EXPECT_EQ(6u + 4, GetFieldCount(dbLockInfo));
		AssertEqualBaseLockInfoData(lockInfo, dbLockInfo);

		EXPECT_EQ(lockInfo.HashAlgorithm, static_cast<model::LockHashAlgorithm>(GetUint8(dbLockInfo, "hashAlgorithm")));
		EXPECT_EQ(lockInfo.Secret, GetHashValue(dbLockInfo, "secret"));
		EXPECT_EQ(lockInfo.RecipientAddress, GetAddressValue(dbLockInfo, "recipientAddress"));
		EXPECT_EQ(lockInfo.CompositeHash, GetHashValue(dbLockInfo, "compositeHash"));
	}
}}
