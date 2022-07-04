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
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::TokenRequiredNotification;

	DEFINE_STATEFUL_VALIDATOR(RequiredToken, [](const Notification& notification, const ValidatorContext& context) {
		auto view = ActiveTokenView(context.Cache);
		auto tokenId = notification.TokenId.resolved(context.Resolvers);

		ActiveTokenView::FindIterator tokenIter;
		auto result = view.tryGet(tokenId, context.Height, notification.Owner.resolved(context.Resolvers), tokenIter);
		if (IsValidationResultFailure(result))
			return result;

		if (0 != notification.PropertyFlagMask) {
			const auto& properties = tokenIter.get().definition().properties();
			for (auto i = 1u; i < utils::to_underlying_type(model::TokenFlags::All); i <<= 1) {
				if (0 != (notification.PropertyFlagMask & i) && !properties.is(static_cast<model::TokenFlags>(i)))
					return Failure_Token_Required_Property_Flag_Unset;
			}
		}

		return ValidationResult::Success;
	})
}}
