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
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace keylink {

	/// Creates a stateful multi key link validator with \a name that validates:
	/// - no conflicting link exists when linking and no more than (\a maxLinks) links
	/// - matching link exists when unlinking
	template<typename TNotification, typename TAccessor>
	validators::stateful::NotificationValidatorPointerT<TNotification> CreateMultiKeyLinkValidator(
			const std::string& name,
			uint8_t maxLinks) {
		using ValidatorType = validators::stateful::FunctionalNotificationValidatorT<TNotification>;
		return std::make_unique<ValidatorType>(name + "MultiKeyLinkValidator", [maxLinks](
				const TNotification& notification,
				const validators::ValidatorContext& context) {
			const auto& cache = context.Cache.sub<cache::AccountStateCache>();
			auto accountStateIter = cache.find(notification.MainAccountPublicKey);
			const auto& accountState = accountStateIter.get();

			const auto& publicKeysAccessor = TAccessor::Get(accountState);
			if (model::LinkAction::Link == notification.LinkAction) {
				if (maxLinks == publicKeysAccessor.size())
					return TAccessor::Failure_Too_Many_Links;

				if (publicKeysAccessor.upperBound() >= notification.LinkedPublicKey.StartEpoch)
					return TAccessor::Failure_Link_Already_Exists;
			} else {
				if (!publicKeysAccessor.containsExact(notification.LinkedPublicKey))
					return TAccessor::Failure_Inconsistent_Unlink_Data;
			}

			return validators::ValidationResult::Success;
		});
	}
}}
