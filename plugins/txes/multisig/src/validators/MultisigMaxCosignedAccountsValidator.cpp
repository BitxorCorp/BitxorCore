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
#include "src/cache/MultisigCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::MultisigNewCosignatoryNotification;

	DECLARE_STATEFUL_VALIDATOR(MultisigMaxCosignedAccounts, Notification)(uint32_t maxCosignedAccountsPerAccount) {
		return MAKE_STATEFUL_VALIDATOR(MultisigMaxCosignedAccounts, [maxCosignedAccountsPerAccount](
				const Notification& notification,
				const ValidatorContext& context) {
			const auto& multisigCache = context.Cache.sub<cache::MultisigCache>();
			auto multisigIter = multisigCache.find(context.Resolvers.resolve(notification.Cosignatory));

			if (!multisigIter.tryGet())
				return ValidationResult::Success;

			const auto& cosignatoryEntry = multisigIter.get();
			return cosignatoryEntry.multisigAddresses().size() >= maxCosignedAccountsPerAccount
					? Failure_Multisig_Max_Cosigned_Accounts
					: ValidationResult::Success;
		});
	}
}}
