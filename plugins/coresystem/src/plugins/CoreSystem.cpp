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

#include "CoreSystem.h"
#include "VotingKeyLinkTransactionPlugin.h"
#include "VrfKeyLinkTransactionPlugin.h"
#include "src/importance/StorageImportanceCalculatorFactory.h"
#include "src/observers/Observers.h"
#include "src/validators/KeyLinkValidators.h"
#include "src/validators/Validators.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/cache_core/AccountStateCacheStorage.h"
#include "bitxorcore/cache_core/AccountStateCacheSubCachePlugin.h"
#include "bitxorcore/cache_core/BlockStatisticCacheStorage.h"
#include "bitxorcore/cache_core/BlockStatisticCacheSubCachePlugin.h"
#include "bitxorcore/keylink/KeyLinkObserver.h"
#include "bitxorcore/keylink/KeyLinkValidator.h"
#include "bitxorcore/keylink/MultiKeyLinkObserver.h"
#include "bitxorcore/keylink/MultiKeyLinkValidator.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/plugins/CacheHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	namespace {
		// region caches

		cache::AccountStateCacheTypes::Options CreateAccountStateCacheOptions(const model::BlockchainConfiguration& config) {
			return {
				config.Network.Identifier,
				config.ImportanceGrouping,
				config.VotingSetGrouping,
				config.MinHarvesterBalance,
				config.MaxHarvesterBalance,
				config.MinVoterBalance,
				config.CurrencyTokenId,
				config.HarvestingTokenId
			};
		}

		void AddAccountStateCache(PluginManager& manager, const model::BlockchainConfiguration& config) {
			using namespace bitxorcore::cache;

			auto cacheConfig = manager.cacheConfig(AccountStateCache::Name);
			auto cacheOptions = CreateAccountStateCacheOptions(config);
			manager.addCacheSupport(std::make_unique<AccountStateCacheSubCachePlugin>(cacheConfig, cacheOptions));

			using CacheHandlers = CacheHandlers<cache::AccountStateCacheDescriptor>;
			CacheHandlers::Register<model::FacilityCode::Core>(manager);

			manager.addDiagnosticCounterHook([](auto& counters, const BitxorCoreCache& cache) {
				counters.emplace_back(utils::DiagnosticCounterId("ACNTST C"), [&cache]() {
					return cache.sub<AccountStateCache>().createView()->size();
				});
				counters.emplace_back(utils::DiagnosticCounterId("ACNTST C HVA"), [&cache]() {
					return cache.sub<AccountStateCache>().createView()->highValueAccounts().addresses().size();
				});
			});
		}

		void AddBlockStatisticCache(PluginManager& manager, const model::BlockchainConfiguration& config) {
			using namespace bitxorcore::cache;

			manager.addCacheSupport(std::make_unique<BlockStatisticCacheSubCachePlugin>(config.MaxDifficultyBlocks));

			manager.addDiagnosticCounterHook([](auto& counters, const BitxorCoreCache& cache) {
				counters.emplace_back(utils::DiagnosticCounterId("BLKDIF C"), [&cache]() {
					return cache.sub<BlockStatisticCache>().createView()->size();
				});
			});
		}

		// endregion

		// region observers

		auto CreateRecalculateImportancesObserver(
				const model::BlockchainConfiguration& config,
				const config::BitxorCoreDirectory& directory) {
			auto pCommitCalculator = importance::CreateImportanceCalculator(config);
			auto pRollbackCalculator = importance::CreateRestoreImportanceCalculator();

			if (0 == config.MaxRollbackBlocks) {
				// enable deep rollbacks
				importance::StorageImportanceCalculatorFactory calculatorFactory(config);
				pCommitCalculator = calculatorFactory.createWriteCalculator(std::move(pCommitCalculator), directory.dir("wip"));
				pRollbackCalculator = calculatorFactory.createReadCalculator(std::move(pRollbackCalculator), directory);
			}

			return observers::CreateRecalculateImportancesObserver(std::move(pCommitCalculator), std::move(pRollbackCalculator));
		}

		// endregion

		// region key link registrations

		struct BasicKeyAccessor {
			static constexpr auto Failure_Link_Already_Exists = validators::Failure_Core_Link_Already_Exists;
			static constexpr auto Failure_Inconsistent_Unlink_Data = validators::Failure_Core_Inconsistent_Unlink_Data;
			static constexpr auto Failure_Too_Many_Links = validators::Failure_Core_Too_Many_Links;
		};

		struct VrfKeyAccessor : public BasicKeyAccessor {
			template<typename TAccountState>
			static auto& Get(TAccountState& accountState) {
				return accountState.SupplementalPublicKeys.vrf();
			}
		};

		struct VotingKeyAccessor : public BasicKeyAccessor {
			template<typename TAccountState>
			static auto& Get(TAccountState& accountState) {
				return accountState.SupplementalPublicKeys.voting();
			}
		};

		void RegisterVrfKeyLinkTransaction(PluginManager& manager) {
			manager.addTransactionSupport(CreateVrfKeyLinkTransactionPlugin());

			manager.addStatefulValidatorHook([](auto& builder) {
				builder.add(keylink::CreateKeyLinkValidator<model::VrfKeyLinkNotification, VrfKeyAccessor>("Vrf"));
			});

			manager.addObserverHook([](auto& builder) {
				builder.add(keylink::CreateKeyLinkObserver<model::VrfKeyLinkNotification, VrfKeyAccessor>("Vrf"));
			});
		}

		void RegisterVotingKeyLinkTransaction(PluginManager& manager) {
			const auto& config = manager.config();

			manager.addTransactionSupport(CreateVotingKeyLinkTransactionPlugin());

			manager.addStatelessValidatorHook([&config](auto& builder) {
				builder.add(validators::CreateVotingKeyLinkRangeValidator(config.MinVotingKeyLifetime, config.MaxVotingKeyLifetime));
			});

			manager.addStatefulValidatorHook([&config](auto& builder) {
				builder.add(keylink::CreateMultiKeyLinkValidator<model::VotingKeyLinkNotification, VotingKeyAccessor>(
						"Voting",
						config.MaxVotingKeysPerAccount));
			});

			manager.addObserverHook([](auto& builder) {
				builder.add(keylink::CreateMultiKeyLinkObserver<model::VotingKeyLinkNotification, VotingKeyAccessor>("Voting"));
			});
		}

		// endregion
	}

	void RegisterCoreSystem(PluginManager& manager) {
		const auto& config = manager.config();

		AddAccountStateCache(manager, config);
		AddBlockStatisticCache(manager, config);

		manager.addStatelessValidatorHook([&config](auto& builder) {
			builder
				.add(validators::CreateZeroAddressValidator(config.Network.Identifier))
				.add(validators::CreateZeroPublicKeyValidator())
				.add(validators::CreateMaxTransactionsValidator(config.MaxTransactionsPerBlock))
				.add(validators::CreateNetworkValidator(config.Network.Identifier))
				.add(validators::CreateEntityVersionValidator())
				.add(validators::CreateTransactionFeeValidator())
				.add(validators::CreateKeyLinkActionValidator())
				.add(validators::CreateZeroInternalPaddingValidator())
				.add(validators::CreateBlockTypeValidator(config.ImportanceGrouping));
		});

		manager.addStatefulValidatorHook([&config](auto& builder) {
			builder
				.add(validators::CreateAddressValidator())
				.add(validators::CreatePublicKeyValidator())
				.add(validators::CreateDeadlineValidator(config.MaxTransactionLifetime))
				.add(validators::CreateGenesisSinkValidator(
						config.ForkHeights.TreasuryReissuance,
						config.TreasuryReissuanceTransactionSignatures))
				.add(validators::CreateEligibleHarvesterValidator())
				.add(validators::CreateBalanceDebitValidator())
				.add(validators::CreateBalanceTransferValidator())
				.add(validators::CreateImportanceBlockValidator(
						config.ForkHeights.TotalVotingBalanceCalculationFix,
						config.VotingSetGrouping));
		});

		auto harvestFeeOptions = observers::HarvestFeeOptions{
			config.CurrencyTokenId,
			config.HarvestBeneficiaryPercentage,
			config.HarvestNetworkPercentage,
			config.HarvestControlStakePercentage,
			model::GetHarvestNetworkFeeSinkAddress(config),
			model::GetHarvestControlStakeFeeSinkAddress(config)
		};
		const auto& calculator = manager.inflationConfig().InflationCalculator;
		manager.addObserverHook([harvestFeeOptions, &calculator](auto& builder) {
			builder
				.add(observers::CreateSourceChangeObserver())
				.add(observers::CreateAccountAddressObserver())
				.add(observers::CreateAccountPublicKeyObserver())
				.add(observers::CreateBalanceDebitObserver())
				.add(observers::CreateBalanceTransferObserver())
				.add(observers::CreateBeneficiaryObserver())
				.add(observers::CreateTransactionFeeActivityObserver())
				.add(observers::CreateHarvestFeeObserver(harvestFeeOptions, calculator))
				.add(observers::CreateTotalTransactionsObserver());
		});

		auto dataDirectory = config::BitxorCoreDataDirectory(manager.userConfig().DataDirectory);
		manager.addTransientObserverHook([&config, dataDirectory](auto& builder) {
			// important:
			// HighValueAccountObserver and RecalculateImportancesObserver are both triggered by BlockNotification and must execute
			// AFTER all state changes.
			// Since transient observers are guaranteed to not cause any state changes, the aforementioned observers can be safely
			// registered as transient observers independent of any transient observers registered by other plugins.
			builder
				.add(observers::CreateHighValueAccountObserver(observers::NotifyMode::Commit))
				.add(CreateRecalculateImportancesObserver(config, dataDirectory.dir("importance")))
				.add(observers::CreateHighValueAccountObserver(observers::NotifyMode::Rollback))
				.add(observers::CreateBlockStatisticObserver(config.MaxDifficultyBlocks, config.DefaultDynamicFeeMultiplier));
		});

		RegisterVrfKeyLinkTransaction(manager);
		RegisterVotingKeyLinkTransaction(manager);
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterCoreSystem(manager);
}
