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
#include "src/model/TokenNotifications.h"
#include "bitxorcore/model/Notifications.h"
#include "bitxorcore/validators/ValidatorTypes.h"
#include <unordered_set>

namespace bitxorcore { namespace validators {

	// region TokenChangeTransaction

	/// Validator that applies to token required notifications and validates that:
	/// - token exists and is active
	/// - token owner matches requesting signer
	DECLARE_STATEFUL_VALIDATOR(RequiredToken, model::TokenRequiredNotification)();

	// endregion

	// region TokenDefinitionTransaction

	/// Validator that applies to token nonce notifications and validates that:
	/// - token id is the expected id generated from signer and nonce
	DECLARE_STATELESS_VALIDATOR(TokenId, model::TokenNonceNotification)();

	/// Validator that applies to token properties notifications and validates that:
	/// - definition has valid token flags
	/// - definition prior to \a revokableForkHeight does not have Revokable flag
	DECLARE_STATEFUL_VALIDATOR(TokenFlags, model::TokenPropertiesNotification)(Height revokableForkHeight);

	/// Validator that applies to token definition notifications and validates that:
	/// - the token is available and can be created or modified
	DECLARE_STATEFUL_VALIDATOR(TokenAvailability, model::TokenDefinitionNotification)();

	/// Validator that applies to token definition notifications and validates that:
	/// - the resulting token duration is no greater than \a maxTokenDuration and there was no overflow
	DECLARE_STATEFUL_VALIDATOR(TokenDuration, model::TokenDefinitionNotification)(BlockDuration maxTokenDuration);

	/// Validator that applies to token definition notifications and validates that:
	/// - the resulting token divisibility is no greater than \a maxDivisibility
	DECLARE_STATEFUL_VALIDATOR(TokenDivisibility, model::TokenDefinitionNotification)(uint8_t maxDivisibility);

	// endregion

	// region TokenSupplyChangeTransaction

	/// Validator that applies to token supply change notifications and validates that:
	/// - action has a valid value
	/// - delta amount is non-zero
	DECLARE_STATELESS_VALIDATOR(TokenSupplyChange, model::TokenSupplyChangeNotification)();

	/// Validator that applies to all balance transfer notifications and validates that:
	/// - transferred token is active and is transferable
	/// - as an optimization, special currency token (\a currencyTokenId) transfers are always allowed
	DECLARE_STATEFUL_VALIDATOR(TokenTransfer, model::BalanceTransferNotification)(UnresolvedTokenId currencyTokenId);

	/// Validator that applies to token supply change notifications and validates that:
	/// - the affected token has mutable supply
	/// - decrease does not cause owner amount to become negative
	/// - increase does not cause total atomic units to exceed \a maxAtomicUnits
	/// \note This validator is dependent on RequiredTokenValidator.
	DECLARE_STATEFUL_VALIDATOR(TokenSupplyChangeAllowed, model::TokenSupplyChangeNotification)(Amount maxAtomicUnits);

	/// Validator that applies to token supply change notifications and validates that:
	/// - the account changing the supply does not exceed the maximum number of tokens (\a maxTokens) an account is allowed to own
	DECLARE_STATEFUL_VALIDATOR(MaxTokensSupplyChange, model::TokenSupplyChangeNotification)(uint16_t maxTokens);

	// endregion

	// region TransferTransaction

	/// Validator that applies to all balance transfer notifications and validates that:
	/// - the recipient does not exceed the maximum number of tokens (\a maxTokens) an account is allowed to own
	DECLARE_STATEFUL_VALIDATOR(MaxTokensBalanceTransfer, model::BalanceTransferNotification)(uint16_t maxTokens);

	// endregion
}}
