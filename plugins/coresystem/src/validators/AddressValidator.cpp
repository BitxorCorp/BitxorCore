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
#include "bitxorcore/model/Address.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::AccountAddressNotification;

	DEFINE_STATEFUL_VALIDATOR(Address, [](const Notification& notification, const ValidatorContext& context) {
		auto networkIdentifier = context.Network.Identifier;
		auto address = notification.Address.resolved(context.Resolvers);
		if (utils::to_underlying_type(networkIdentifier) != (uint16_t(((uint16_t)(address[0]) << 8) | (uint16_t)(address[1]))))
			return Failure_Core_Invalid_Address;

		auto isValidAddress = model::IsValidAddress(address, networkIdentifier);
		return isValidAddress ? ValidationResult::Success : Failure_Core_Invalid_Address;
	})
}}
