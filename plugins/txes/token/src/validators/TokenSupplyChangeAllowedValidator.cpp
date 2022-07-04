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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::TokenSupplyChangeNotification;

	DECLARE_STATEFUL_VALIDATOR(TokenSupplyChangeAllowed, Notification)(Amount maxAtomicUnits) {
		return MAKE_STATEFUL_VALIDATOR(TokenSupplyChangeAllowed, [maxAtomicUnits](
				const Notification& notification,
				const ValidatorContext& context) {
			// notice that RequiredTokenValidator will run first, so both token and owning account must exist
			auto tokenId = context.Resolvers.resolve(notification.TokenId);
			const auto& cache = context.Cache.sub<cache::TokenCache>();
			auto tokenIter = cache.find(tokenId);
			const auto& tokenEntry = tokenIter.get();

			const auto& accountStateCache = context.Cache.sub<cache::AccountStateCache>();
			auto accountStateIter = accountStateCache.find(notification.Owner);
			auto ownerAmount = accountStateIter.get().Balances.get(tokenId);

			// only allow an "immutable" supply to change if the owner owns full supply
			const auto& properties = tokenEntry.definition().properties();
			if (!properties.is(model::TokenFlags::Supply_Mutable) && ownerAmount != tokenEntry.supply())
				return Failure_Token_Supply_Immutable;

			if (model::TokenSupplyChangeAction::Decrease == notification.Action)
				return ownerAmount < notification.Delta ? Failure_Token_Supply_Negative : ValidationResult::Success;

			// check that new supply does not overflow and is not too large
			auto initialSupply = tokenEntry.supply();
			auto newSupply = tokenEntry.supply() + notification.Delta;
			return newSupply < initialSupply || newSupply > maxAtomicUnits
					? Failure_Token_Supply_Exceeded
					: ValidationResult::Success;
		});
	}
}}
