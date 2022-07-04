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
#include "src/model/HashLockNotifications.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace validators {

	/// Validator that applies to hash lock notifications and validates that:
	/// - lock duration is at most \a maxHashLockDuration
	DECLARE_STATELESS_VALIDATOR(HashLockDuration, model::HashLockDurationNotification)(BlockDuration maxHashLockDuration);

	/// Validator that applies to hash lock token notifications and validates that:
	/// - token id is \a currencyTokenId
	/// - token amount is equal to \a lockedFundsPerAggregate
	DECLARE_STATEFUL_VALIDATOR(HashLockToken, model::HashLockTokenNotification)(
			TokenId currencyTokenId,
			Amount lockedFundsPerAggregate);

	/// Validator that applies to hash lock notifications and validates that:
	/// - attached hash is not present in hash lock info cache
	DECLARE_STATEFUL_VALIDATOR(HashLockCacheUnique, model::HashLockNotification)();

	/// Validator that applies to transaction notifications and validates that:
	/// - incomplete aggregate transactions must have an active, unused hash lock info present in cache
	DECLARE_STATEFUL_VALIDATOR(AggregateHashPresent, model::TransactionNotification)();
}}
