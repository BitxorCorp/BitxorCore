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
#include "TokenRestrictionCacheTypes.h"

namespace bitxorcore { namespace cache {

	// region GetTokenGlobalRestrictionResolvedRules

	/// Result of token global restriction rule resolution.
	enum class TokenGlobalRestrictionRuleResolutionResult {
		/// All rules are valid and have been resolved.
		Success,

		/// No rules are set.
		No_Rules,

		/// At least one rule is invalid.
		Invalid_Rule
	};

	/// Resolved token restriction rule.
	struct TokenRestrictionResolvedRule {
		/// Identifier of the token providing the restriction key.
		bitxorcore::TokenId TokenId;

		/// Restriction key.
		uint64_t RestrictionKey;

		/// Restriction value.
		uint64_t RestrictionValue;

		/// Restriction type.
		model::TokenRestrictionType RestrictionType;
	};

	/// Gets the (resolved) global restriction rules for the token identified by \a tokenId using \a restrictionCache
	/// and inserts resolved rules into \a resolvedRules.
	TokenGlobalRestrictionRuleResolutionResult GetTokenGlobalRestrictionResolvedRules(
			const TokenRestrictionCacheTypes::CacheReadOnlyType& restrictionCache,
			TokenId tokenId,
			std::vector<TokenRestrictionResolvedRule>& resolvedRules);

	// endregion

	// region EvaluateTokenRestrictionResolvedRulesForAddress

	/// Evaluates all resolved rules (\a resolvedRules) for the account identified by \a address using \a restrictionCache.
	bool EvaluateTokenRestrictionResolvedRulesForAddress(
			const TokenRestrictionCacheTypes::CacheReadOnlyType& restrictionCache,
			const Address& address,
			const std::vector<TokenRestrictionResolvedRule>& resolvedRules);

	// endregion
}}
