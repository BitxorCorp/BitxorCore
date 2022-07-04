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
#include "src/model/TokenRestrictionNotifications.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace validators {

	/// Validator that applies to token restriction type notifications and validates that:
	/// - restriction type is known
	DECLARE_STATELESS_VALIDATOR(TokenRestrictionType, model::TokenRestrictionTypeNotification)();

	/// Validator that applies to token restriction required notifications and validates that:
	/// - corresponding global restriction exists
	DECLARE_STATEFUL_VALIDATOR(TokenRestrictionRequired, model::TokenRestrictionRequiredNotification)();

	/// Validator that applies to new value notifications of token global restriction modifications and validates that:
	/// - the requested modification will not cause the number of restrictions to exceed \a maxTokenRestrictionValues
	/// - delete only applies to existing values
	DECLARE_STATEFUL_VALIDATOR(
			TokenGlobalRestrictionMaxValues,
			model::TokenGlobalRestrictionModificationNewValueNotification)(uint8_t maxTokenRestrictionValues);

	/// Validator that applies to new value notifications of token address restriction modifications and validates that:
	/// - the requested modification will not cause the number of restrictions to exceed \a maxTokenRestrictionValues
	/// - delete only applies to existing values
	DECLARE_STATEFUL_VALIDATOR(
			TokenAddressRestrictionMaxValues,
			model::TokenAddressRestrictionModificationNewValueNotification)(uint8_t maxTokenRestrictionValues);

	/// Validator that applies to previous value notifications of token global restriction modifications and validates that:
	/// - the specified previous value(s) match the current value(s)
	DECLARE_STATEFUL_VALIDATOR(TokenGlobalRestrictionModification, model::TokenGlobalRestrictionModificationPreviousValueNotification)();

	/// Validator that applies to previous value notifications of token address restriction modifications and validates that:
	/// - the specified previous value(s) match the current value(s)
	DECLARE_STATEFUL_VALIDATOR(
			TokenAddressRestrictionModification,
			model::TokenAddressRestrictionModificationPreviousValueNotification)();

	/// Validator that applies to balance transfer notifications and validates that:
	/// - sender is authorized to send token
	/// - recipient is authorized to receive token
	DECLARE_STATEFUL_VALIDATOR(TokenRestrictionBalanceTransfer, model::BalanceTransferNotification)();

	/// Validator that applies to balance debit notifications and validates that:
	/// - recipient is authorized to receive token
	DECLARE_STATEFUL_VALIDATOR(TokenRestrictionBalanceDebit, model::BalanceDebitNotification)();
}}
