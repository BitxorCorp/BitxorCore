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

#include "MemoryHashCacheSystem.h"
#include "src/cache/HashCacheStorage.h"
#include "src/handlers/HashCacheDiagnosticHandlers.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/observers/ObserverUtils.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	void RegisterMemoryHashCacheSystem(PluginManager& manager) {
		const auto& config = manager.config();

		manager.addCacheSupport<cache::HashCacheStorage>(std::make_unique<cache::HashCache>(
				manager.cacheConfig(cache::HashCache::Name),
				CalculateTransactionCacheDuration(config)));

		manager.addDiagnosticHandlerHook([](auto& handlers, const cache::BitxorCoreCache& cache) {
			handlers::RegisterConfirmTimestampedHashesHandler(
					handlers,
					handlers::CreateConfirmedTimestampedHashesProducerFactory(cache.sub<cache::HashCache>()));
		});

		manager.addDiagnosticCounterHook([](auto& counters, const cache::BitxorCoreCache& cache) {
			counters.emplace_back(utils::DiagnosticCounterId("HASH C"), [&cache]() {
				return cache.sub<cache::HashCache>().createView()->size();
			});
		});

		manager.addStatefulValidatorHook([](auto& builder) {
			builder.add(validators::CreateUniqueTransactionHashValidator());
		});

		manager.addTransientObserverHook([](auto& builder) {
			builder.add(observers::CreateTransactionHashObserver());
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterMemoryHashCacheSystem(manager);
}
