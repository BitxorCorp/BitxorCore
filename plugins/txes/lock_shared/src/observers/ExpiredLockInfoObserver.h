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

	/// On commit, credits the expiration account(s) of expired locks and creates receipts of \a receiptType.
	/// On rollback, debits the expiration account(s) of expired locks.
	/// Uses the observer \a context to determine notification direction and access caches.
	/// Uses \a ownerAccountStateDispatcher to retrieve the lock owner's account state.
	template<typename TLockInfoCache, typename TAccountStateDispatcher>
	void ExpiredLockInfoObserver(
			ObserverContext& context,
			model::ReceiptType receiptType,
			TAccountStateDispatcher ownerAccountStateDispatcher) {
		std::vector<std::unique_ptr<model::BalanceChangeReceipt>> receipts;
		auto receiptAppender = [&receipts, receiptType](const auto& address, auto tokenId, auto amount) {
			receipts.push_back(std::make_unique<model::BalanceChangeReceipt>(receiptType, address, tokenId, amount));
		};

		auto& lockInfoCache = context.Cache.template sub<TLockInfoCache>();
		lockInfoCache.processUnusedExpiredLocks(context.Height, [&context, ownerAccountStateDispatcher, receiptAppender](
				const auto& lockInfo) {
			auto& accountStateCache = context.Cache.sub<cache::AccountStateCache>();
			ownerAccountStateDispatcher(accountStateCache, lockInfo, [&context, &lockInfo, receiptAppender](auto& accountState) {
				if (NotifyMode::Rollback == context.Mode) {
					accountState.Balances.debit(lockInfo.TokenId, lockInfo.Amount);
					return;
				}

				accountState.Balances.credit(lockInfo.TokenId, lockInfo.Amount);
				receiptAppender(accountState.Address, lockInfo.TokenId, lockInfo.Amount);
			});
		});

		// sort receipts in order to fulfill deterministic ordering requirement
		std::sort(receipts.begin(), receipts.end(), [](const auto& pLhs, const auto& pRhs) {
			return std::memcmp(pLhs.get(), pRhs.get(), sizeof(model::BalanceChangeReceipt)) < 0;
		});

		for (const auto& pReceipt : receipts)
			context.StatementBuilder().addReceipt(*pReceipt);
	}
}}
