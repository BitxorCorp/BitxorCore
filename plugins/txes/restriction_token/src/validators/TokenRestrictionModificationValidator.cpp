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

#include "Validators.h"
#include "src/cache/TokenRestrictionCache.h"
#include "src/state/TokenAddressRestrictionNotificationFacade.h"
#include "src/state/TokenGlobalRestrictionNotificationFacade.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	namespace {
		using GlobalNotificationFacade = state::TokenGlobalRestrictionNotificationFacade<
			model::TokenGlobalRestrictionModificationPreviousValueNotification::Notification_Type>;
		using AddressNotificationFacade = state::TokenAddressRestrictionNotificationFacade<
			model::TokenAddressRestrictionModificationPreviousValueNotification::Notification_Type>;

		template<typename TNotificationFacade>
		ValidationResult TokenRestrictionModificationValidator(
				const typename TNotificationFacade::NotificationType& notification,
				const ValidatorContext& context) {
			TNotificationFacade notificationFacade(notification, context.Resolvers);

			const auto& cache = context.Cache.sub<cache::TokenRestrictionCache>();
			auto entryIter = cache.find(notificationFacade.uniqueKey());

			typename TNotificationFacade::RuleType rule;
			if (entryIter.tryGet() && notificationFacade.tryGet(entryIter.get(), rule)) {
				if (!notificationFacade.isMatch(rule))
					return Failure_RestrictionToken_Previous_Value_Mismatch;
			} else {
				if (!notificationFacade.isUnset())
					return Failure_RestrictionToken_Previous_Value_Must_Be_Zero;
			}

			return ValidationResult::Success;
		}
	}

#define DEFINE_TOKEN_RESTRICTION_MODIFICATION_VALIDATOR(NAME) \
	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(Token##NAME##RestrictionModification, NAME##NotificationFacade::NotificationType, ([]( \
			const NAME##NotificationFacade::NotificationType& notification, \
			const ValidatorContext& context) { \
		return TokenRestrictionModificationValidator<NAME##NotificationFacade>(notification, context); \
	}))

	DEFINE_TOKEN_RESTRICTION_MODIFICATION_VALIDATOR(Global)
	DEFINE_TOKEN_RESTRICTION_MODIFICATION_VALIDATOR(Address)
}}
