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

#include "TokenRestrictionCacheUtils.h"
#include "TokenRestrictionCacheDelta.h"
#include "TokenRestrictionCacheView.h"
#include "src/state/TokenRestrictionEvaluator.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"
#include "bitxorcore/utils/Hashers.h"

namespace bitxorcore { namespace cache {

	// region GetTokenGlobalRestrictionResolvedRules

	TokenGlobalRestrictionRuleResolutionResult GetTokenGlobalRestrictionResolvedRules(
			const TokenRestrictionCacheTypes::CacheReadOnlyType& restrictionCache,
			TokenId tokenId,
			std::vector<TokenRestrictionResolvedRule>& resolvedRules) {
		auto entryIter = restrictionCache.find(state::CreateTokenRestrictionEntryKey(tokenId));
		if (!entryIter.tryGet())
			return TokenGlobalRestrictionRuleResolutionResult::No_Rules;

		const auto& restriction = entryIter.get().asGlobalRestriction();
		for (auto key : restriction.keys()) {
			state::TokenGlobalRestriction::RestrictionRule rule;
			restriction.tryGet(key, rule);

			if (tokenId == rule.ReferenceTokenId)
				return TokenGlobalRestrictionRuleResolutionResult::Invalid_Rule;

			resolvedRules.push_back(TokenRestrictionResolvedRule{
				TokenId() == rule.ReferenceTokenId ? tokenId : rule.ReferenceTokenId,
				key,
				rule.RestrictionValue,
				rule.RestrictionType
			});
		}

		return TokenGlobalRestrictionRuleResolutionResult::Success;
	}

	// endregion

	// region EvaluateTokenRestrictionResolvedRulesForAddress

	bool EvaluateTokenRestrictionResolvedRulesForAddress(
			const TokenRestrictionCacheTypes::CacheReadOnlyType& restrictionCache,
			const Address& address,
			const std::vector<TokenRestrictionResolvedRule>& resolvedRules) {
		std::unordered_map<TokenId, state::TokenAddressRestriction, utils::BaseValueHasher<TokenId>> restrictionCacheSet;
		for (const auto& resolvedRule : resolvedRules) {
			auto tokenId = resolvedRule.TokenId;
			auto restrictionIter = restrictionCacheSet.find(tokenId);
			if (restrictionCacheSet.cend() == restrictionIter) {
				auto entryIter = restrictionCache.find(state::CreateTokenRestrictionEntryKey(tokenId, address));
				auto restriction = entryIter.tryGet()
						? entryIter.get().asAddressRestriction()
						: state::TokenAddressRestriction(tokenId, address);
				restrictionIter = restrictionCacheSet.emplace(tokenId, restriction).first;
			}

			auto evaluateResult = state::EvaluateTokenRestriction(
					{ TokenId(), resolvedRule.RestrictionValue, resolvedRule.RestrictionType },
					restrictionIter->second.get(resolvedRule.RestrictionKey));
			if (!evaluateResult)
				return false;
		}

		return true;
	}

	// endregion
}}
