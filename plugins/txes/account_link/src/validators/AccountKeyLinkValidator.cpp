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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::RemoteAccountKeyLinkNotification;

	DEFINE_STATEFUL_VALIDATOR(AccountKeyLink, [](const Notification& notification, const ValidatorContext& context) {
		const auto& cache = context.Cache.sub<cache::AccountStateCache>();
		auto accountStateIter = cache.find(notification.MainAccountPublicKey);
		const auto& accountState = accountStateIter.get();

		if (model::LinkAction::Link == notification.LinkAction) {
			if (state::AccountType::Unlinked != accountState.AccountType)
				return Failure_AccountLink_Link_Already_Exists;
		} else {
			// only main accounts can unlink (not remotes)
			if (state::AccountType::Main != accountState.AccountType)
				return Failure_AccountLink_Unknown_Link;

			if (notification.LinkedPublicKey != state::GetLinkedPublicKey(accountState))
				return Failure_AccountLink_Inconsistent_Unlink_Data;
		}

		return ValidationResult::Success;
	})
}}
