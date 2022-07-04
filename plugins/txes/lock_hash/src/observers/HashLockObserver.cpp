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

#include "Observers.h"
#include "src/cache/HashLockInfoCache.h"
#include "src/model/HashLockReceiptType.h"
#include "src/state/HashLockInfo.h"

namespace bitxorcore { namespace observers {

	using Notification = model::HashLockNotification;

	namespace {
		auto CreateLockInfo(const Address& owner, TokenId tokenId, Height endHeight, const Notification& notification) {
			return state::HashLockInfo(owner, tokenId, notification.Token.Amount, endHeight, notification.Hash);
		}
	}

	DEFINE_OBSERVER(HashLock, Notification, [](const Notification& notification, ObserverContext& context) {
		auto& cache = context.Cache.sub<cache::HashLockInfoCache>();
		if (NotifyMode::Commit == context.Mode) {
			auto endHeight = context.Height + Height(notification.Duration.unwrap());
			auto tokenId = context.Resolvers.resolve(notification.Token.TokenId);
			if (cache.isActive(notification.Hash, context.Height))
				BITXORCORE_THROW_INVALID_ARGUMENT("cannot add already active hash lock");

			cache.insert(CreateLockInfo(notification.Owner, tokenId, endHeight, notification));

			auto receiptType = model::Receipt_Type_LockHash_Created;
			model::BalanceChangeReceipt receipt(receiptType, notification.Owner, tokenId, notification.Token.Amount);
			context.StatementBuilder().addReceipt(receipt);
		} else {
			cache.remove(notification.Hash);
		}
	})
}}
