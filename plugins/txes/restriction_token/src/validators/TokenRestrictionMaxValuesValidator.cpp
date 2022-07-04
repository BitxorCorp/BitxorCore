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
			model::TokenGlobalRestrictionModificationNewValueNotification::Notification_Type>;
		using AddressNotificationFacade = state::TokenAddressRestrictionNotificationFacade<
			model::TokenAddressRestrictionModificationNewValueNotification::Notification_Type>;

		size_t GetRestrictionCount(const state::TokenRestrictionEntry& entry) {
			return state::TokenRestrictionEntry::EntryType::Global == entry.entryType()
					? entry.asGlobalRestriction().size()
					: entry.asAddressRestriction().size();
		}

		template<typename TNotificationFacade>
		ValidationResult TokenRestrictionMaxValuesValidator(
				uint8_t maxTokenRestrictionValues,
				const typename TNotificationFacade::NotificationType& notification,
				const ValidatorContext& context) {
			TNotificationFacade notificationFacade(notification, context.Resolvers);
			auto isDeleteAction = notificationFacade.isDeleteAction();

			const auto& cache = context.Cache.sub<cache::TokenRestrictionCache>();
			auto entryIter = cache.find(notificationFacade.uniqueKey());
			if (!entryIter.tryGet())
				return isDeleteAction ? Failure_RestrictionToken_Cannot_Delete_Nonexistent_Restriction : ValidationResult::Success;

			typename TNotificationFacade::RuleType rule;
			auto isNewRule = !notificationFacade.tryGet(entryIter.get(), rule);
			if (isDeleteAction && isNewRule)
				return Failure_RestrictionToken_Cannot_Delete_Nonexistent_Restriction;

			return isDeleteAction || !isNewRule || GetRestrictionCount(entryIter.get()) < maxTokenRestrictionValues
					? ValidationResult::Success
					: Failure_RestrictionToken_Max_Restrictions_Exceeded;
		}
	}

#define DEFINE_TOKEN_RESTRICTION_MAX_VALUES_VALIDATOR(NAME) \
	DECLARE_STATEFUL_VALIDATOR(Token##NAME##RestrictionMaxValues, NAME##NotificationFacade::NotificationType)( \
			uint8_t maxTokenRestrictionValues) { \
		using ValidatorType = stateful::FunctionalNotificationValidatorT<NAME##NotificationFacade::NotificationType>; \
		return std::make_unique<ValidatorType>("Token" #NAME "RestrictionMaxValuesValidator", [maxTokenRestrictionValues]( \
				const NAME##NotificationFacade::NotificationType& notification, \
				const ValidatorContext& context) { \
			return TokenRestrictionMaxValuesValidator<NAME##NotificationFacade>(maxTokenRestrictionValues, notification, context); \
		}); \
	}

	DEFINE_TOKEN_RESTRICTION_MAX_VALUES_VALIDATOR(Global)
	DEFINE_TOKEN_RESTRICTION_MAX_VALUES_VALIDATOR(Address)
}}
