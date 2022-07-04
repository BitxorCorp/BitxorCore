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

#include "KeyLinkValidators.h"
#include "bitxorcore/validators/ValidatorUtils.h"

namespace bitxorcore { namespace validators {

	using Notification = model::VotingKeyLinkNotification;

	namespace {
		bool IsOutsideRange(const Notification& notification, uint32_t minRange, uint32_t maxRange) {
			const auto& pinnedVotingKey = notification.LinkedPublicKey;
			if (pinnedVotingKey.EndEpoch < pinnedVotingKey.StartEpoch)
				return true;

			auto range = (pinnedVotingKey.EndEpoch - pinnedVotingKey.StartEpoch).unwrap() + 1;
			return range < minRange || range > maxRange;
		}
	}

	DECLARE_STATELESS_VALIDATOR(VotingKeyLinkRange, Notification)(uint32_t minRange, uint32_t maxRange) {
		return MAKE_STATELESS_VALIDATOR(VotingKeyLinkRange, ([minRange, maxRange](const Notification& notification) {
			if (FinalizationEpoch() == notification.LinkedPublicKey.StartEpoch)
				return Failure_Core_Link_Start_Epoch_Invalid;

			return IsOutsideRange(notification, minRange, maxRange) ? Failure_Core_Invalid_Link_Range : ValidationResult::Success;
		}));
	}
}}
