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
#include "bitxorcore/cache_core/AccountStateCache.h"

namespace bitxorcore { namespace observers {

	/// On commit, marks lock as used and credits destination account.
	/// On rollback, marks lock as unused and debits destination account.
	/// Uses the observer \a context to determine notification direction and access caches.
	/// Uses \a notification to determine the destination account.
	template<typename TTraits>
	void LockStatusAccountBalanceObserver(const typename TTraits::Notification& notification, ObserverContext& context) {
		auto& accountStateCache = context.Cache.sub<cache::AccountStateCache>();
		auto& cache = context.Cache.template sub<typename TTraits::CacheType>();
		const auto& key = TTraits::NotificationToKey(notification, context.Resolvers);
		auto lockInfoIter = cache.find(key);
		auto& lockInfo = lockInfoIter.get().back();

		auto accountStateIter = accountStateCache.find(TTraits::DestinationAccount(lockInfo));
		auto& accountState = accountStateIter.get();

		if (NotifyMode::Rollback == context.Mode) {
			lockInfo.Status = state::LockStatus::Unused;
			accountState.Balances.debit(lockInfo.TokenId, lockInfo.Amount);
			return;
		}

		lockInfo.Status = state::LockStatus::Used;
		accountState.Balances.credit(lockInfo.TokenId, lockInfo.Amount);

		model::BalanceChangeReceipt receipt(TTraits::Receipt_Type, accountState.Address, lockInfo.TokenId, lockInfo.Amount);
		context.StatementBuilder().addReceipt(receipt);
	}
}}
