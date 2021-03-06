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
#include "src/cache/TokenRestrictionCache.h"
#include "src/state/TokenAddressRestrictionNotificationFacade.h"
#include "src/state/TokenGlobalRestrictionNotificationFacade.h"

namespace bitxorcore { namespace observers {

	namespace {
		using GlobalCommitNotification = model::TokenGlobalRestrictionModificationNewValueNotification;
		using GlobalRollbackNotification = model::TokenGlobalRestrictionModificationPreviousValueNotification;
		using AddressCommitNotification = model::TokenAddressRestrictionModificationNewValueNotification;
		using AddressRollbackNotification = model::TokenAddressRestrictionModificationPreviousValueNotification;

		template<typename TNotificationFacade>
		void ObserveNotification(
				NotifyMode requiredMode,
				const typename TNotificationFacade::NotificationType& notification,
				const ObserverContext& context) {
			if (requiredMode != context.Mode)
				return;

			TNotificationFacade notificationFacade(notification, context.Resolvers);

			auto& cache = context.Cache.sub<cache::TokenRestrictionCache>();
			auto entryIter = cache.find(notificationFacade.uniqueKey());

			if (!entryIter.tryGet()) {
				auto entry = state::TokenRestrictionEntry(notificationFacade.toRestriction());
				notificationFacade.update(entry);
				cache.insert(entry);
				return;
			}

			auto& entry = entryIter.get();
			auto numRules = notificationFacade.update(entry);
			if (0 == numRules)
				cache.remove(entry.uniqueKey());
		}
	}

#define DEFINE_TOKEN_RESTRICTION_MODIFICATION_OBSERVER(NAME, MODE) \
	DEFINE_OBSERVER(Token##NAME##Restriction##MODE##Modification, NAME##MODE##Notification, ([]( \
			const NAME##MODE##Notification& notification, \
			const ObserverContext& context) { \
		using NotificationFacade = state::Token##NAME##RestrictionNotificationFacade<NAME##MODE##Notification::Notification_Type>; \
		ObserveNotification<NotificationFacade>(NotifyMode::MODE, notification, context); \
	}))

	DEFINE_TOKEN_RESTRICTION_MODIFICATION_OBSERVER(Global, Commit)
	DEFINE_TOKEN_RESTRICTION_MODIFICATION_OBSERVER(Global, Rollback)

	DEFINE_TOKEN_RESTRICTION_MODIFICATION_OBSERVER(Address, Commit)
	DEFINE_TOKEN_RESTRICTION_MODIFICATION_OBSERVER(Address, Rollback)
}}
