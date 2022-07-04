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
#include "src/cache/TokenRestrictionCacheUtils.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	namespace {
		using AddressRulesEvaluator = predicate<
			const cache::TokenRestrictionCacheTypes::CacheReadOnlyType&,
			const std::vector<cache::TokenRestrictionResolvedRule>>;

		ValidationResult ProcessTokenRules(
				UnresolvedTokenId unresolvedTokenId,
				const ValidatorContext& context,
				const AddressRulesEvaluator& addressRulesEvaluator) {
			const auto& cache = context.Cache.sub<cache::TokenRestrictionCache>();

			auto tokenId = context.Resolvers.resolve(unresolvedTokenId);

			std::vector<cache::TokenRestrictionResolvedRule> tokenRules;
			auto result = cache::GetTokenGlobalRestrictionResolvedRules(cache, tokenId, tokenRules);

			if (cache::TokenGlobalRestrictionRuleResolutionResult::No_Rules == result)
				return ValidationResult::Success;

			if (cache::TokenGlobalRestrictionRuleResolutionResult::Invalid_Rule == result)
				return Failure_RestrictionToken_Invalid_Global_Restriction;

			return addressRulesEvaluator(cache, tokenRules)
					? ValidationResult::Success
					: Failure_RestrictionToken_Account_Unauthorized;
		}

		ValidationResult CheckTransferAuthorization(
				const model::BalanceTransferNotification& notification,
				const ValidatorContext& context) {
			return ProcessTokenRules(notification.TokenId, context, [&notification, &resolvers = context.Resolvers](
					const auto& cache,
					const auto& tokenRules) {
				auto isSenderAuthorized = cache::EvaluateTokenRestrictionResolvedRulesForAddress(
						cache,
						notification.Sender.resolved(resolvers),
						tokenRules);
				auto isRecipientAuthorized = cache::EvaluateTokenRestrictionResolvedRulesForAddress(
						cache,
						notification.Recipient.resolved(resolvers),
						tokenRules);
				return isSenderAuthorized && isRecipientAuthorized;
			});
		}

		ValidationResult CheckDebitAuthorization(const model::BalanceDebitNotification& notification, const ValidatorContext& context) {
			return ProcessTokenRules(notification.TokenId, context, [&notification, &resolvers = context.Resolvers](
					const auto& cache,
					const auto& tokenRules) {
				return cache::EvaluateTokenRestrictionResolvedRulesForAddress(
						cache,
						notification.Sender.resolved(resolvers),
						tokenRules);
			});
		}
	}

	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(TokenRestrictionBalanceTransfer, model::BalanceTransferNotification, CheckTransferAuthorization)
	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(TokenRestrictionBalanceDebit, model::BalanceDebitNotification, CheckDebitAuthorization)
}}
