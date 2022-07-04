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
#include "plugins/txes/lock_shared/src/observers/ExpiredLockInfoObserver.h"
#include "bitxorcore/cache_core/AccountStateCacheUtils.h"
#include "bitxorcore/observers/ObserverUtils.h"

namespace bitxorcore { namespace observers {

	DEFINE_OBSERVER(ExpiredHashLockInfo, model::BlockNotification, [](
			const model::BlockNotification& notification,
			ObserverContext& context) {
		ExpiredLockInfoObserver<cache::HashLockInfoCache>(context, model::Receipt_Type_LockHash_Expired, [&notification](
				auto& accountStateCache,
				const auto&,
				auto accountStateConsumer) {
			cache::ProcessForwardedAccountState(accountStateCache, notification.Harvester, [accountStateConsumer](auto& accountState) {
				accountStateConsumer(accountState);
			});
		});
	})
}}
