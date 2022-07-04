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
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::MultisigCosignatoriesNotification;

	DEFINE_STATEFUL_VALIDATOR(MultisigInvalidCosignatories, [](const Notification& notification, const ValidatorContext& context) {
		const auto& multisigCache = context.Cache.sub<cache::MultisigCache>();
		auto multisigIter = multisigCache.find(notification.Multisig);

		if (!multisigIter.tryGet())
			return 0 == notification.AddressDeletionsCount ? ValidationResult::Success : Failure_Multisig_Unknown_Multisig_Account;

		const auto& multisigEntry = multisigIter.get();
		for (auto i = 0u; i < notification.AddressAdditionsCount; ++i) {
			if (multisigEntry.hasCosignatory(context.Resolvers.resolve(notification.AddressAdditionsPtr[i])))
				return Failure_Multisig_Already_A_Cosignatory;
		}

		for (auto i = 0u; i < notification.AddressDeletionsCount; ++i) {
			if (!multisigEntry.hasCosignatory(context.Resolvers.resolve(notification.AddressDeletionsPtr[i])))
				return Failure_Multisig_Not_A_Cosignatory;
		}

		return ValidationResult::Success;
	})
}}
