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
#include "src/cache/TokenRestrictionCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	namespace {
		using Notification = model::TokenRestrictionRequiredNotification;

		bool GlobalRestrictionExists(const Notification& notification, const ValidatorContext& context) {
			const auto& cache = context.Cache.sub<cache::TokenRestrictionCache>();
			auto entryIter = cache.find(state::CreateTokenRestrictionEntryKey(context.Resolvers.resolve(notification.TokenId)));

			// entry must wrap a global restriction because key is created with zero address
			state::TokenGlobalRestriction::RestrictionRule rule;
			return entryIter.tryGet() && entryIter.get().asGlobalRestriction().tryGet(notification.RestrictionKey, rule);
		}
	}

	DEFINE_STATEFUL_VALIDATOR(TokenRestrictionRequired, ([](const Notification& notification, const ValidatorContext& context) {
		return GlobalRestrictionExists(notification, context)
				? ValidationResult::Success
				: Failure_RestrictionToken_Unknown_Global_Restriction;
	}))
}}
