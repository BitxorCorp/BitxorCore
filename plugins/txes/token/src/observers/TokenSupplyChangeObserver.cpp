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
#include "src/cache/TokenCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"

namespace bitxorcore { namespace observers {

	using Notification = model::TokenSupplyChangeNotification;

	namespace {
		constexpr bool ShouldIncrease(NotifyMode mode, model::TokenSupplyChangeAction action) {
			return
					(NotifyMode::Commit == mode && model::TokenSupplyChangeAction::Increase == action) ||
					(NotifyMode::Rollback == mode && model::TokenSupplyChangeAction::Decrease == action);
		}
	}

	DEFINE_OBSERVER(TokenSupplyChange, Notification, [](const Notification& notification, const ObserverContext& context) {
		auto tokenId = context.Resolvers.resolve(notification.TokenId);
		auto& accountStateCache = context.Cache.sub<cache::AccountStateCache>();
		auto& tokenCache = context.Cache.sub<cache::TokenCache>();

		auto accountStateIter = accountStateCache.find(notification.Owner);
		auto& accountState = accountStateIter.get();

		auto tokenIter = tokenCache.find(tokenId);
		auto& tokenEntry = tokenIter.get();
		if (ShouldIncrease(context.Mode, notification.Action)) {
			accountState.Balances.credit(tokenId, notification.Delta);
			tokenEntry.increaseSupply(notification.Delta);
		} else {
			accountState.Balances.debit(tokenId, notification.Delta);
			tokenEntry.decreaseSupply(notification.Delta);
		}
	})
}}
