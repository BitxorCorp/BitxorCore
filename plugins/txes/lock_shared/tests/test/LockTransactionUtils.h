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
#include "sdk/src/extensions/ConversionExtensions.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	/// Asserts that lock \a notification properties match with corresponding properties of \a transaction.
	template<typename TBaseLockNotification, typename TTransaction>
	void AssertBaseLockNotification(const TBaseLockNotification& notification, const TTransaction& transaction) {
		EXPECT_EQ(model::GetSignerAddress(transaction), notification.Owner);
		EXPECT_EQ(transaction.Token.TokenId, notification.Token.TokenId);
		EXPECT_EQ(transaction.Token.Amount, notification.Token.Amount);
		EXPECT_EQ(transaction.Duration, notification.Duration);
	}

	/// Creates a random lock transaction based on \a TTraits.
	template<typename TTraits>
	auto CreateRandomLockTransaction() {
		using TransactionType = typename TTraits::TransactionType;
		auto entitySize = sizeof(TransactionType);
		auto pTransaction = utils::MakeUniqueWithSize<typename TTraits::TransactionType>(entitySize);
		test::FillWithRandomData({ reinterpret_cast<uint8_t*>(pTransaction.get()), entitySize });
		pTransaction->Size = static_cast<uint32_t>(entitySize);
		return pTransaction;
	}
}}
