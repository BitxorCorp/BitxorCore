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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/state/AccountBalances.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	namespace {
		template<typename TAccountIdentifier>
		ValidationResult CheckAccount(
				uint16_t maxTokens,
				TokenId tokenId,
				const TAccountIdentifier& accountIdentifier,
				const ValidatorContext& context) {
			const auto& accountStateCache = context.Cache.sub<cache::AccountStateCache>();
			auto accountStateIter = accountStateCache.find(accountIdentifier);
			const auto& balances = accountStateIter.get().Balances;
			if (balances.get(tokenId) != Amount())
				return ValidationResult::Success;

			return maxTokens <= balances.size() ? Failure_Token_Max_Tokens_Exceeded : ValidationResult::Success;
		}
	}

	DECLARE_STATEFUL_VALIDATOR(MaxTokensBalanceTransfer, model::BalanceTransferNotification)(uint16_t maxTokens) {
		using ValidatorType = stateful::FunctionalNotificationValidatorT<model::BalanceTransferNotification>;
		auto name = "MaxTokensBalanceTransferValidator";
		return std::make_unique<ValidatorType>(name, [maxTokens](
				const model::BalanceTransferNotification& notification,
				const ValidatorContext& context) {
			if (Amount() == notification.Amount)
				return ValidationResult::Success;

			return CheckAccount(
					maxTokens,
					context.Resolvers.resolve(notification.TokenId),
					notification.Recipient.resolved(context.Resolvers),
					context);
		});
	}

	DECLARE_STATEFUL_VALIDATOR(MaxTokensSupplyChange, model::TokenSupplyChangeNotification)(uint16_t maxTokens) {
		using ValidatorType = stateful::FunctionalNotificationValidatorT<model::TokenSupplyChangeNotification>;
		auto name = "MaxTokensSupplyChangeValidator";
		return std::make_unique<ValidatorType>(name, [maxTokens](
				const model::TokenSupplyChangeNotification& notification,
				const ValidatorContext& context) {
			if (model::TokenSupplyChangeAction::Decrease == notification.Action)
				return ValidationResult::Success;

			return CheckAccount(maxTokens, context.Resolvers.resolve(notification.TokenId), notification.Owner, context);
		});
	}
}}
