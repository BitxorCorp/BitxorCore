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
#include "src/state/AccountRestrictions.h"

namespace bitxorcore { namespace validators {

	using Notification = model::AccountRestrictionModificationNotification;

	namespace {
		bool IsValidAccountRestrictionFlags(model::AccountRestrictionFlags restrictionFlags) {
			auto strippedRestrictionFlags = state::AccountRestrictionDescriptor(restrictionFlags).restrictionFlags();
			auto directionalRestrictionFlags = state::AccountRestrictionDescriptor(restrictionFlags).directionalRestrictionFlags();
			switch (strippedRestrictionFlags) {
			case model::AccountRestrictionFlags::Address:
				return true;

			case model::AccountRestrictionFlags::TokenId:
				return HasSingleFlag(directionalRestrictionFlags);

			case model::AccountRestrictionFlags::TransactionType:
				return HasFlag(model::AccountRestrictionFlags::Outgoing, directionalRestrictionFlags);

			default:
				return false;
			}
		}
	}

	DEFINE_STATELESS_VALIDATOR(AccountRestrictionFlags, [](const Notification& notification) {
		return IsValidAccountRestrictionFlags(notification.RestrictionFlags)
				? ValidationResult::Success
				: Failure_RestrictionAccount_Invalid_Restriction_Flags;
	})
}}
