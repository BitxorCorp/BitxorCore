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

#include "TokenRestrictionEvaluator.h"
#include "TokenAddressRestriction.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace state {

	namespace {
		bool Evaluate(const TokenGlobalRestriction::RestrictionRule& rule, uint64_t value, bool& isValidRule) {
			isValidRule = true;
			switch (rule.RestrictionType) {
			case model::TokenRestrictionType::EQ:
				return rule.RestrictionValue == value;

			case model::TokenRestrictionType::NE:
				return rule.RestrictionValue != value;

			case model::TokenRestrictionType::LT:
				return rule.RestrictionValue > value;

			case model::TokenRestrictionType::LE:
				return rule.RestrictionValue >= value;

			case model::TokenRestrictionType::GT:
				return rule.RestrictionValue < value;

			case model::TokenRestrictionType::GE:
				return rule.RestrictionValue <= value;

			default:
				break;
			}

			isValidRule = false;
			return false;
		}
	}

	bool EvaluateTokenRestriction(const TokenGlobalRestriction::RestrictionRule& rule, uint64_t value) {
		bool isValidRule;
		auto evaluateResult = Evaluate(rule, value, isValidRule);

		if (!isValidRule) {
			BITXORCORE_LOG(error)
					<< "cannot evaluate token restriction rule with unsupported type "
					<< static_cast<uint16_t>(rule.RestrictionType);
			return false;
		}

		// if unset value, only NE should match
		// otherwise, use result of evaluation
		return TokenAddressRestriction::Sentinel_Removal_Value == value
				? model::TokenRestrictionType::NE == rule.RestrictionType
				: evaluateResult;
	}
}}
