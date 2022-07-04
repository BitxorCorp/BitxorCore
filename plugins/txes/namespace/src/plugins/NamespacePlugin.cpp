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

#include "NamespacePlugin.h"
#include "AddressAliasTransactionPlugin.h"
#include "TokenAliasTransactionPlugin.h"
#include "NamespaceRegistrationTransactionPlugin.h"
#include "src/cache/NamespaceCache.h"
#include "src/cache/NamespaceCacheStorage.h"
#include "src/cache/NamespaceCacheSubCachePlugin.h"
#include "src/config/NamespaceConfiguration.h"
#include "src/model/NamespaceLifetimeConstraints.h"
#include "src/model/NamespaceReceiptType.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/observers/ObserverUtils.h"
#include "bitxorcore/observers/RentalFeeObserver.h"
#include "bitxorcore/plugins/CacheHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	namespace {
		NamespaceAliasFeeConfiguration ToNamespaceAliasFeeConfiguration(
				const model::BlockchainConfiguration& blockchainConfig,
				UnresolvedTokenId currencyTokenId,
				const config::NamespaceConfiguration& config) {
			NamespaceAliasFeeConfiguration rentalFeeConfig;
			rentalFeeConfig.CurrencyTokenId = currencyTokenId;
			//rentalFeeConfig.RootFeePerBlock = config.RootNamespaceRentalFeePerBlock;
			rentalFeeConfig.UnlinkRentalFee = config.UnlinkTokenAliasRentalFee;
			//rentalFeeConfig.EternalRentalFee = config.RootNamespaceEternalFee;
			rentalFeeConfig.GenesisSignerPublicKey = blockchainConfig.Network.GenesisSignerPublicKey;
			rentalFeeConfig.SinkAddress = config::GetNamespaceRentalFeeSinkAddress(config, blockchainConfig);
			return rentalFeeConfig;
		}
		// region alias

		void RegisterAliasSubsystem(PluginManager& manager, const config::NamespaceConfiguration& config) {
			auto currencyTokenId = model::GetUnresolvedCurrencyTokenId(manager.config());
			auto unlinkFeeConfig = ToNamespaceAliasFeeConfiguration(manager.config(), currencyTokenId, config);
			manager.addTransactionSupport(CreateAddressAliasTransactionPlugin());
			manager.addTransactionSupport(CreateTokenAliasTransactionPlugin(unlinkFeeConfig));

			manager.addStatelessValidatorHook([](auto& builder) {
				builder.add(validators::CreateAliasActionValidator());
			});

			manager.addStatefulValidatorHook([](auto& builder) {
				builder
					.add(validators::CreateAliasAvailabilityValidator())
					.add(validators::CreateUnlinkAliasedAddressConsistencyValidator())
					.add(validators::CreateUnlinkAliasedTokenIdConsistencyValidator())
					.add(validators::CreateAddressAliasValidator());
			});

			manager.addObserverHook([](auto& builder) {
				builder
					.add(observers::CreateAliasedAddressObserver())
					.add(observers::CreateAliasedTokenIdObserver());
			});
		}

		// endregion

		// region namespace

		NamespaceRentalFeeConfiguration ToNamespaceRentalFeeConfiguration(
				const model::BlockchainConfiguration& blockchainConfig,
				UnresolvedTokenId currencyTokenId,
				const config::NamespaceConfiguration& config) {
			NamespaceRentalFeeConfiguration rentalFeeConfig;
			rentalFeeConfig.CurrencyTokenId = currencyTokenId;
			rentalFeeConfig.RootFeePerBlock = config.RootNamespaceRentalFeePerBlock;
			rentalFeeConfig.ChildFee = config.ChildNamespaceRentalFee;
			rentalFeeConfig.MaxBlockDuration = config.MaxNamespaceDuration.blocks(blockchainConfig.BlockGenerationTargetTime);
			rentalFeeConfig.EternalRentalFee = config.RootNamespaceEternalFee;
			rentalFeeConfig.GenesisSignerPublicKey = blockchainConfig.Network.GenesisSignerPublicKey;
			rentalFeeConfig.SinkAddress = config::GetNamespaceRentalFeeSinkAddress(config, blockchainConfig);
			return rentalFeeConfig;
		}

		template<typename TAliasValue, typename TAliasValueAccessor>
		bool RunNamespaceResolver(
				const cache::NamespaceCacheTypes::CacheReadOnlyType& namespaceCache,
				NamespaceId namespaceId,
				state::AliasType aliasType,
				TAliasValue& aliasValue,
				TAliasValueAccessor aliasValueAccessor) {
			auto iter = namespaceCache.find(namespaceId);
			if (!iter.tryGet())
				return false;

			const auto& alias = iter.get().root().alias(namespaceId);
			if (aliasType != alias.type())
				return false;

			aliasValue = aliasValueAccessor(alias);
			return true;
		}

		void RegisterNamespaceAliasResolvers(PluginManager& manager) {
			manager.addTokenResolver([](const auto&, const auto& unresolved, auto& resolved) {
				constexpr uint64_t Namespace_Flag = 1ull << 63;
				if (0 == (Namespace_Flag & unresolved.unwrap())) {
					resolved = model::ResolverContext().resolve(unresolved);
					return true;
				}

				return false;
			});

			manager.addTokenResolver([](const auto& cache, const auto& unresolved, auto& resolved) {
				auto namespaceCache = cache.template sub<cache::NamespaceCache>();
				auto namespaceId = NamespaceId(unresolved.unwrap());
				return RunNamespaceResolver(namespaceCache, namespaceId, state::AliasType::Token, resolved, [](const auto& alias) {
					return alias.tokenId();
				});
			});

			manager.addAddressResolver([](const auto&, const auto& unresolved, auto& resolved) {
				if (0 == (1 & unresolved[0])) {
					resolved = model::ResolverContext().resolve(unresolved);
					return true;
				}

				return false;
			});

			manager.addAddressResolver([](const auto& cache, const auto& unresolved, auto& resolved) {
				auto namespaceCache = cache.template sub<cache::NamespaceCache>();
				NamespaceId namespaceId;
				std::memcpy(static_cast<void*>(&namespaceId), unresolved.data() + 1, sizeof(NamespaceId));
				return RunNamespaceResolver(namespaceCache, namespaceId, state::AliasType::Address, resolved, [](const auto& alias) {
					return alias.address();
				});
			});
		}

		auto GetNamespaceView(const cache::BitxorCoreCache& cache) {
			return cache.sub<cache::NamespaceCache>().createView();
		}

		void RegisterNamespaceSubsystem(PluginManager& manager, const config::NamespaceConfiguration& config) {
			auto currencyTokenId = model::GetUnresolvedCurrencyTokenId(manager.config());
			auto rentalFeeConfig = ToNamespaceRentalFeeConfiguration(manager.config(), currencyTokenId, config);
			manager.addTransactionSupport(CreateNamespaceRegistrationTransactionPlugin(rentalFeeConfig));

			auto minDuration = config.MinNamespaceDuration.blocks(manager.config().BlockGenerationTargetTime);
			auto maxDuration = config.MaxNamespaceDuration.blocks(manager.config().BlockGenerationTargetTime);
			auto gracePeriodDuration = config.NamespaceGracePeriodDuration.blocks(manager.config().BlockGenerationTargetTime);
			model::NamespaceLifetimeConstraints constraints(maxDuration, gracePeriodDuration);

			RegisterNamespaceAliasResolvers(manager);
			manager.addCacheSupport(std::make_unique<cache::NamespaceCacheSubCachePlugin>(
					manager.cacheConfig(cache::NamespaceCache::Name),
					cache::NamespaceCacheTypes::Options{ gracePeriodDuration }));

			using CacheHandlers = CacheHandlers<cache::NamespaceCacheDescriptor>;
			CacheHandlers::Register<model::FacilityCode::Namespace>(manager);

			manager.addDiagnosticCounterHook([](auto& counters, const cache::BitxorCoreCache& cache) {
				counters.emplace_back(utils::DiagnosticCounterId("NS C"), [&cache]() { return GetNamespaceView(cache)->size(); });
				counters.emplace_back(utils::DiagnosticCounterId("NS C AS"), [&cache]() { return GetNamespaceView(cache)->activeSize(); });
				counters.emplace_back(utils::DiagnosticCounterId("NS C DS"), [&cache]() { return GetNamespaceView(cache)->deepSize(); });
			});

			manager.addStatelessValidatorHook([config, minDuration, maxDuration](auto& builder) {
				builder
					.add(validators::CreateNamespaceRegistrationTypeValidator())
					.add(validators::CreateNamespaceNameValidator(config.MaxNameSize))
					.add(validators::CreateRootNamespaceValidator(minDuration, maxDuration));
			});

			manager.addStatefulValidatorHook([constraints, config](auto& builder) {
				builder
					.add(validators::CreateNamespaceReservedNameValidator(config.ReservedRootNamespaceNames))
					.add(validators::CreateRootNamespaceAvailabilityValidator())
					.add(validators::CreateNamespaceDurationOverflowValidator(constraints.MaxNamespaceDuration))
					// note that the following validator needs to run before the RootNamespaceMaxChildrenValidator
					.add(validators::CreateChildNamespaceAvailabilityValidator(config.MaxNamespaceDepth))
					.add(validators::CreateRootNamespaceMaxChildrenValidator(config.MaxChildNamespaces))
					.add(validators::CreateRequiredNamespaceValidator());
			});

			auto maxRollbackBlocks = BlockDuration(manager.config().MaxRollbackBlocks);
			manager.addObserverHook([gracePeriodDuration, maxRollbackBlocks](auto& builder) {
				auto rentalFeeReceiptType = model::Receipt_Type_Namespace_Rental_Fee;
				auto expiryReceiptType = model::Receipt_Type_Namespace_Deleted;
				builder
					.add(observers::CreateRootNamespaceObserver())
					.add(observers::CreateChildNamespaceObserver())
					.add(observers::CreateRentalFeeObserver<model::NamespaceRentalFeeNotification>("Namespace", rentalFeeReceiptType))
					.add(observers::CreateCacheBlockTouchObserver<cache::NamespaceCache>(
							"NamespaceGracePeriod",
							model::Receipt_Type_Namespace_Expired,
							gracePeriodDuration))
					.add(observers::CreateCacheBlockTouchObserver<cache::NamespaceCache>("Namespace", expiryReceiptType));
			});
		}

		// endregion
	}

	void RegisterNamespaceSubsystem(PluginManager& manager) {
		auto config = model::LoadPluginConfiguration<config::NamespaceConfiguration>(manager.config(), "bitxorcore.plugins.namespace");
		RegisterNamespaceSubsystem(manager, config);
		RegisterAliasSubsystem(manager, config);
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterNamespaceSubsystem(manager);
}
