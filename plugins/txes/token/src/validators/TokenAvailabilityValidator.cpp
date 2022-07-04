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
#include "ActiveTokenView.h"
#include "src/cache/TokenCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::TokenDefinitionNotification;

	namespace {
		bool ContainsAnyPropertyChange(const state::TokenDefinition& definition, const model::TokenProperties& properties) {
			// if any property modified by XOR is nonzero, there is a property change
			auto xorPropertyChanged = model::TokenFlags::None != properties.flags() || 0 != properties.divisibility();
			if (xorPropertyChanged)
				return true;

			// duration deltas only affect tokens that are not eternal
			// (if any change is made to an eternal duration, it will get rejected by the TokenDurationValidator)
			return !definition.isEternal() && BlockDuration() != properties.duration();
		}
	}

	DEFINE_STATEFUL_VALIDATOR(TokenAvailability, [](const Notification& notification, const ValidatorContext& context) {
		auto view = ActiveTokenView(context.Cache);

		// token has to be active
		ActiveTokenView::FindIterator tokenIter;
		auto result = view.tryGet(notification.TokenId, context.Height, notification.Owner, tokenIter);

		// always allow a new token
		if (IsValidationResultFailure(result))
			return tokenIter.tryGet() ? result : ValidationResult::Success;

		// disallow a noop modification
		const auto& tokenEntry = tokenIter.get();
		if (!ContainsAnyPropertyChange(tokenEntry.definition(), notification.Properties))
			return Failure_Token_Modification_No_Changes;

		// require token supply to be zero because else, when rolling back, the definition observer does not know
		// what the supply was before
		return Amount() != tokenEntry.supply() ? Failure_Token_Modification_Disallowed : ValidationResult::Success;
	})
}}
