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

#pragma once
#include "plugins/txes/lock_shared/src/state/LockInfo.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "mongo/tests/test/MapperTestUtils.h"
#include "tests/TestHarness.h"
#include <mongocxx/client.hpp>

namespace bitxorcore { namespace test {

	/// Assert that non-inherited fields of a lock \a transaction match with corresponding fields of \a dbTransaction.
	template<typename TTransaction>
	void AssertEqualNonInheritedLockTransactionData(const TTransaction& transaction, const bsoncxx::document::view& dbTransaction) {
		EXPECT_EQ(transaction.Duration, BlockDuration(test::GetUint64(dbTransaction, "duration")));
		EXPECT_EQ(transaction.Token.TokenId, UnresolvedTokenId(test::GetUint64(dbTransaction, "tokenId")));
		EXPECT_EQ(transaction.Token.Amount, Amount(test::GetUint64(dbTransaction, "amount")));
	}

	/// Asserts that \a lockInfo is equal to \a dbLockInfo.
	inline void AssertEqualBaseLockInfoData(const state::LockInfo& lockInfo, const bsoncxx::document::view& dbLockInfo) {
		EXPECT_EQ(1u, GetUint32(dbLockInfo, "version"));

		EXPECT_EQ(lockInfo.OwnerAddress, GetAddressValue(dbLockInfo, "ownerAddress"));
		EXPECT_EQ(lockInfo.TokenId, TokenId(GetUint64(dbLockInfo, "tokenId")));
		EXPECT_EQ(lockInfo.Amount, Amount(GetUint64(dbLockInfo, "amount")));
		EXPECT_EQ(lockInfo.EndHeight, Height(GetUint64(dbLockInfo, "endHeight")));
		EXPECT_EQ(lockInfo.Status, static_cast<state::LockStatus>(GetUint8(dbLockInfo, "status")));
	}
}}
