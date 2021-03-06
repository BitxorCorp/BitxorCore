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
#include "bitxorcore/model/VerifiableEntity.h"

namespace bitxorcore { namespace validators {

	using Notification = model::TransactionFeeNotification;

	DEFINE_STATELESS_VALIDATOR(TransactionFee, [](const Notification& notification) {
		if (notification.Fee > notification.MaxFee)
			return Failure_Core_Invalid_Transaction_Fee;

		constexpr auto Max_Raw_Block_Fee_Multiplier = static_cast<uint64_t>(std::numeric_limits<BlockFeeMultiplier::ValueType>::max());
		return notification.MaxFee.unwrap() > Max_Raw_Block_Fee_Multiplier * notification.TransactionSize
				? Failure_Core_Invalid_Transaction_Fee
				: ValidationResult::Success;
	})
}}
