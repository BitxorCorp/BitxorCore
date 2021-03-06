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
#include "Results.h"
#include "src/model/KeyLinkNotifications.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace validators {

	/// Validator that applies to key link action notifications and validates that:
	/// - link action is valid
	DECLARE_STATELESS_VALIDATOR(KeyLinkAction, model::KeyLinkActionNotification)();

	/// Validator that applies to voting key link notifications and validates that:
	/// - start point is prior to end point
	/// - range is longer than \a minRange
	/// - range is shorter than \a maxRange
	DECLARE_STATELESS_VALIDATOR(VotingKeyLinkRange, model::VotingKeyLinkNotification)(uint32_t minRange, uint32_t maxRange);
}}
