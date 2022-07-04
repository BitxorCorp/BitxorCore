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

#include "TokenPlugin.h"
#include "TokenDefinitionTransactionPlugin.h"
#include "TokenSupplyChangeTransactionPlugin.h"
#include "TokenSupplyRevocationTransactionPlugin.h"
#include "src/cache/TokenCache.h"
#include "src/cache/TokenCacheStorage.h"
#include "src/config/TokenConfiguration.h"
#include "src/model/TokenReceiptType.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/observers/ObserverUtils.h"
#include "bitxorcore/observers/RentalFeeObserver.h"
#include "bitxorcore/plugins/CacheHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	namespace {
		TokenRentalFeeConfiguration ToTokenRentalFeeConfiguration(
				const model::BlockchainConfiguration& blockchainConfig,
				UnresolvedTokenId currencyTokenId,
				const config::TokenConfiguration& config) {
			TokenRentalFeeConfiguration rentalFeeConfig;
			rentalFeeConfig.CurrencyTokenId = currencyTokenId;
			rentalFeeConfig.Fee = config.TokenRentalFee;
			rentalFeeConfig.GenesisSignerPublicKey = blockchainConfig.Network.GenesisSignerPublicKey;
			rentalFeeConfig.SinkAddress = config::GetTokenRentalFeeSinkAddress(config, blockchainConfig);
			return rentalFeeConfig;
		}

		auto GetTokenView(const cache::BitxorCoreCache& cache) {
			return cache.sub<cache::TokenCache>().createView();
		}
	}

	void RegisterTokenSubsystem(PluginManager& manager) {
		auto config = model::LoadPluginConfiguration<config::TokenConfiguration>(manager.config(), "bitxorcore.plugins.token");
		auto unresolvedCurrencyTokenId = model::GetUnresolvedCurrencyTokenId(manager.config());
		auto rentalFeeConfig = ToTokenRentalFeeConfiguration(manager.config(), unresolvedCurrencyTokenId, config);
		manager.addTransactionSupport(CreateTokenDefinitionTransactionPlugin(rentalFeeConfig));
		manager.addTransactionSupport(CreateTokenSupplyChangeTransactionPlugin());
		manager.addTransactionSupport(CreateTokenSupplyRevocationTransactionPlugin(
				model::GetGenesisSignerAddress(manager.config().Network)));

		manager.addCacheSupport<cache::TokenCacheStorage>(
				std::make_unique<cache::TokenCache>(manager.cacheConfig(cache::TokenCache::Name)));

		using CacheHandlers = CacheHandlers<cache::TokenCacheDescriptor>;
		CacheHandlers::Register<model::FacilityCode::Token>(manager);

		manager.addDiagnosticCounterHook([](auto& counters, const cache::BitxorCoreCache& cache) {
			counters.emplace_back(utils::DiagnosticCounterId("TOKEN C"), [&cache]() { return GetTokenView(cache)->size(); });
		});

		manager.addStatelessValidatorHook([](auto& builder) {
			builder
				.add(validators::CreateTokenIdValidator())
				.add(validators::CreateTokenSupplyChangeValidator());
		});

		manager.addStatefulValidatorHook([config, unresolvedCurrencyTokenId, &networkConfig = manager.config()](auto& builder) {
			builder
				.add(validators::CreateTokenFlagsValidator(networkConfig.ForkHeights.TreasuryReissuance))
				.add(validators::CreateRequiredTokenValidator())
				.add(validators::CreateTokenAvailabilityValidator())
				.add(validators::CreateTokenDivisibilityValidator(config.MaxTokenDivisibility))
				.add(validators::CreateTokenDurationValidator(config.MaxTokenDuration.blocks(networkConfig.BlockGenerationTargetTime)))
				.add(validators::CreateTokenTransferValidator(unresolvedCurrencyTokenId))
				.add(validators::CreateMaxTokensBalanceTransferValidator(config.MaxTokensPerAccount))
				.add(validators::CreateMaxTokensSupplyChangeValidator(config.MaxTokensPerAccount))
				// note that the following validator depends on RequiredTokenValidator
				.add(validators::CreateTokenSupplyChangeAllowedValidator(networkConfig.MaxTokenAtomicUnits));
		});

		auto currencyTokenId = manager.config().CurrencyTokenId;
		const auto& calculator = manager.inflationConfig().InflationCalculator;
		auto maxRollbackBlocks = BlockDuration(manager.config().MaxRollbackBlocks);
		manager.addObserverHook([currencyTokenId, calculator, maxRollbackBlocks](auto& builder) {
			auto rentalFeeReceiptType = model::Receipt_Type_Token_Rental_Fee;
			auto expiryReceiptType = model::Receipt_Type_Token_Expired;
			builder
				.add(observers::CreateTokenDefinitionObserver())
				.add(observers::CreateTokenSupplyChangeObserver())
				.add(observers::CreateTokenSupplyInflationObserver(currencyTokenId, calculator))
				.add(observers::CreateRentalFeeObserver<model::TokenRentalFeeNotification>("Token", rentalFeeReceiptType))
				.add(observers::CreateCacheBlockTouchObserver<cache::TokenCache>("Token", expiryReceiptType));
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterTokenSubsystem(manager);
}
