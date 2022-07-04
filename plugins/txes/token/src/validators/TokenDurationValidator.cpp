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
#include "src/cache/TokenCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::TokenDefinitionNotification;

	DECLARE_STATEFUL_VALIDATOR(TokenDuration, Notification)(BlockDuration maxTokenDuration) {
		return MAKE_STATEFUL_VALIDATOR(TokenDuration, [maxTokenDuration](
				const Notification& notification,
				const ValidatorContext& context) {
			// skip this validator is there is no duration change
			if (BlockDuration() == notification.Properties.duration())
				return ValidationResult::Success;

			// as an optimization, since duration is additive, check notification before checking cache
			if (maxTokenDuration < notification.Properties.duration())
				return Failure_Token_Invalid_Duration;

			// allow a new token because checks above passed
			const auto& cache = context.Cache.sub<cache::TokenCache>();
			auto tokenIter = cache.find(notification.TokenId);
			if (!tokenIter.tryGet())
				return ValidationResult::Success;

			const auto& tokenEntry = tokenIter.get();
			auto currentDuration = tokenEntry.definition().properties().duration();

			// cannot change eternal durations
			if (BlockDuration() == currentDuration)
				return Failure_Token_Invalid_Duration;

			auto resultingDuration = currentDuration + notification.Properties.duration();
			return maxTokenDuration < resultingDuration || resultingDuration < currentDuration
					? Failure_Token_Invalid_Duration
					: ValidationResult::Success;
		});
	}
}}
