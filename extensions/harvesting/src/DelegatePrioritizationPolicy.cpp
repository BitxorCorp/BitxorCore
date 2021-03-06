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

#include "DelegatePrioritizationPolicy.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/cache_core/ImportanceView.h"
#include "bitxorcore/utils/ConfigurationValueParsers.h"
#include "bitxorcore/utils/MacroBasedEnumIncludes.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace harvesting {

	// region DelegatePrioritizationPolicy

#define DEFINE_ENUM DelegatePrioritizationPolicy
#define ENUM_LIST DELEGATE_PRIORITIZATION_POLICY_LIST
#include "bitxorcore/utils/MacroBasedEnum.h"
#undef ENUM_LIST
#undef DEFINE_ENUM

	namespace {
		const std::array<std::pair<const char*, DelegatePrioritizationPolicy>, 2> String_To_Delegate_Prioritization_Policy_Pairs{{
			{ "Age", DelegatePrioritizationPolicy::Age },
			{ "Importance", DelegatePrioritizationPolicy::Importance }
		}};
	}

	bool TryParseValue(const std::string& str, DelegatePrioritizationPolicy& policy) {
		return utils::TryParseEnumValue(String_To_Delegate_Prioritization_Policy_Pairs, str, policy);
	}

	// endregion

	// region CreateDelegatePrioritizer

	namespace {
		DelegatePrioritizer CreateAgePrioritizer() {
			return [](const auto&) {
				// use natural order
				return 0;
			};
		}

		DelegatePrioritizer CreateImportancePrioritizer(const cache::BitxorCoreCache& cache, const Key& primaryAccountPublicKey) {
			return [&cache, primaryAccountPublicKey](const auto& delegatePublicKey) {
				// prevent primary account from getting pruned
				if (primaryAccountPublicKey == delegatePublicKey)
					return std::numeric_limits<uint64_t>::max();

				auto cacheView = cache.createView();
				auto height = cacheView.height() + Height(1); // harvesting *next* block
				auto readOnlyAccountStateCache = cache::ReadOnlyAccountStateCache(cacheView.sub<cache::AccountStateCache>());
				cache::ImportanceView view(readOnlyAccountStateCache);
				return view.getAccountImportanceOrDefault(delegatePublicKey, height).unwrap();
			};
		}
	}

	DelegatePrioritizer CreateDelegatePrioritizer(
			DelegatePrioritizationPolicy policy,
			const cache::BitxorCoreCache& cache,
			const Key& primaryAccountPublicKey) {
		switch (policy) {
		case DelegatePrioritizationPolicy::Age:
			return CreateAgePrioritizer();

		case DelegatePrioritizationPolicy::Importance:
			return CreateImportancePrioritizer(cache, primaryAccountPublicKey);
		}

		BITXORCORE_THROW_INVALID_ARGUMENT_1("cannot create delegate prioritizer for unknown policy", static_cast<uint16_t>(policy));
	}

	// endregion
}}
