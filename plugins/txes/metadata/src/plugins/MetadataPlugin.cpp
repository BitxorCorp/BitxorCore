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

#include "MetadataPlugin.h"
#include "MetadataTransactionPlugin.h"
#include "src/cache/MetadataCache.h"
#include "src/cache/MetadataCacheStorage.h"
#include "src/config/MetadataConfiguration.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/plugins/CacheHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	void RegisterMetadataSubsystem(PluginManager& manager) {
		manager.addTransactionSupport(CreateAccountMetadataTransactionPlugin());
		manager.addTransactionSupport(CreateTokenMetadataTransactionPlugin());
		manager.addTransactionSupport(CreateNamespaceMetadataTransactionPlugin());

		manager.addCacheSupport<cache::MetadataCacheStorage>(std::make_unique<cache::MetadataCache>(
				manager.cacheConfig(cache::MetadataCache::Name)));

		using CacheHandlers = CacheHandlers<cache::MetadataCacheDescriptor>;
		CacheHandlers::Register<model::FacilityCode::Metadata>(manager);

		manager.addDiagnosticCounterHook([](auto& counters, const cache::BitxorCoreCache& cache) {
			counters.emplace_back(utils::DiagnosticCounterId("METADATA C"), [&cache]() {
				return cache.sub<cache::MetadataCache>().createView()->size();
			});
		});

		auto config = model::LoadPluginConfiguration<config::MetadataConfiguration>(manager.config(), "bitxorcore.plugins.metadata");
		manager.addStatelessValidatorHook([maxValueSize = config.MaxValueSize](auto& builder) {
			builder.add(validators::CreateMetadataSizesValidator(maxValueSize));
		});

		manager.addStatefulValidatorHook([](auto& builder) {
			builder.add(validators::CreateMetadataValueValidator());
		});

		manager.addObserverHook([](auto& builder) {
			builder.add(observers::CreateMetadataValueObserver());
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterMetadataSubsystem(manager);
}
