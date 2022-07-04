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

#include "TokenRestrictionPlugin.h"
#include "TokenAddressRestrictionTransactionPlugin.h"
#include "TokenGlobalRestrictionTransactionPlugin.h"
#include "src/cache/TokenRestrictionCache.h"
#include "src/cache/TokenRestrictionCacheStorage.h"
#include "src/config/TokenRestrictionConfiguration.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/plugins/CacheHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	void RegisterTokenRestrictionSubsystem(PluginManager& manager) {
		manager.addTransactionSupport(CreateTokenAddressRestrictionTransactionPlugin());
		manager.addTransactionSupport(CreateTokenGlobalRestrictionTransactionPlugin());

		auto networkIdentifier = manager.config().Network.Identifier;
		manager.addCacheSupport<cache::TokenRestrictionCacheStorage>(std::make_unique<cache::TokenRestrictionCache>(
				manager.cacheConfig(cache::TokenRestrictionCache::Name),
				networkIdentifier));

		using CacheHandlers = CacheHandlers<cache::TokenRestrictionCacheDescriptor>;
		CacheHandlers::Register<model::FacilityCode::RestrictionToken>(manager);

		manager.addDiagnosticCounterHook([](auto& counters, const cache::BitxorCoreCache& cache) {
			counters.emplace_back(utils::DiagnosticCounterId("TOKENREST C"), [&cache]() {
				return cache.sub<cache::TokenRestrictionCache>().createView()->size();
			});
		});

		manager.addStatelessValidatorHook([](auto& builder) {
			builder.add(validators::CreateTokenRestrictionTypeValidator());
		});

		auto config = model::LoadPluginConfiguration<config::TokenRestrictionConfiguration>(
				manager.config(),
				"bitxorcore.plugins.restrictiontoken");
		manager.addStatefulValidatorHook([maxTokenRestrictionValues = config.MaxTokenRestrictionValues](auto& builder) {
			builder
				.add(validators::CreateTokenRestrictionBalanceDebitValidator())
				.add(validators::CreateTokenRestrictionBalanceTransferValidator())
				.add(validators::CreateTokenRestrictionRequiredValidator())
				.add(validators::CreateTokenGlobalRestrictionMaxValuesValidator(maxTokenRestrictionValues))
				.add(validators::CreateTokenGlobalRestrictionModificationValidator())
				.add(validators::CreateTokenAddressRestrictionMaxValuesValidator(maxTokenRestrictionValues))
				.add(validators::CreateTokenAddressRestrictionModificationValidator());
		});

		manager.addObserverHook([](auto& builder) {
			builder
				.add(observers::CreateTokenGlobalRestrictionCommitModificationObserver())
				.add(observers::CreateTokenGlobalRestrictionRollbackModificationObserver())
				.add(observers::CreateTokenAddressRestrictionCommitModificationObserver())
				.add(observers::CreateTokenAddressRestrictionRollbackModificationObserver());
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterTokenRestrictionSubsystem(manager);
}
