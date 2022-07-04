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
#include "HeightDependentAddress.h"
#include "NetworkInfo.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/FileSize.h"
#include "bitxorcore/utils/TimeSpan.h"
#include "bitxorcore/types.h"
#include <unordered_map>
#include <stdint.h>

namespace bitxorcore { namespace model {

	/// Blockchain configuration settings.
	struct BlockchainConfiguration {
	public:
		/// Blockchain network.
		NetworkInfo Network;

		/// \c true if blockchain should calculate state hashes so that state is fully verifiable at each block.
		bool EnableVerifiableState;

		/// \c true if blockchain should calculate receipts so that state changes are fully verifiable at each block.
		bool EnableVerifiableReceipts;

		/// Token id used as primary chain currency.
		TokenId CurrencyTokenId;

		/// Token id used to provide harvesting ability.
		TokenId HarvestingTokenId;

		/// Targeted time between blocks.
		utils::TimeSpan BlockGenerationTargetTime;

		/// Smoothing factor in thousandths.
		/// If this value is non-zero, the network will be biased in favor of evenly spaced blocks.
		/// \note A higher value makes the network more biased.
		/// \note This can lower security because it will increase the influence of time relative to importance.
		uint32_t BlockTimeSmoothingFactor;

		/// Number of blocks that should be treated as a group for importance purposes.
		/// \note Importances will only be calculated at blocks that are multiples of this grouping number.
		uint64_t ImportanceGrouping;

		/// Percentage of importance resulting from fee generation and beneficiary usage.
		uint8_t ImportanceActivityPercentage;

		/// Maximum number of blocks that can be rolled back.
		uint32_t MaxRollbackBlocks;

		/// Maximum number of blocks to use in a difficulty calculation.
		uint32_t MaxDifficultyBlocks;

		/// Default multiplier to use for dynamic fees.
		BlockFeeMultiplier DefaultDynamicFeeMultiplier;

		/// Maximum lifetime a transaction can have before it expires.
		utils::TimeSpan MaxTransactionLifetime;

		/// Maximum future time of a block that can be accepted.
		utils::TimeSpan MaxBlockFutureTime;

		/// Initial currency atomic units available in the network.
		Amount InitialCurrencyAtomicUnits;

		/// Maximum atomic units (total-supply * 10 ^ divisibility) of a token allowed in the network.
		Amount MaxTokenAtomicUnits;

		/// Total whole importance units available in the network.
		Importance TotalChainImportance;

		/// Minimum number of harvesting token atomic units needed for an account to be eligible for harvesting.
		Amount MinHarvesterBalance;

		/// Maximum number of harvesting token atomic units needed for an account to be eligible for harvesting.
		Amount MaxHarvesterBalance;

		/// Minimum number of harvesting token atomic units needed for an account to be eligible for voting.
		Amount MinVoterBalance;

		/// Number of blocks that should be treated as a group for voting set purposes.
		/// \note Voting sets will only be calculated at blocks that are multiples of this grouping number.
		uint64_t VotingSetGrouping;

		/// Maximum number of voting keys that can be registered at once per account.
		uint8_t MaxVotingKeysPerAccount;

		/// Minimum number of finalization rounds for which voting key can be registered.
		uint32_t MinVotingKeyLifetime;

		/// Maximum number of finalization rounds for which voting key can be registered.
		uint32_t MaxVotingKeyLifetime;

		/// Percentage of the harvested fee that is collected by the beneficiary account.
		uint8_t HarvestBeneficiaryPercentage;

		/// Percentage of the harvested fee that is collected by the network.
		uint8_t HarvestNetworkPercentage;
		
		/// Percentage of the harvested fee that is collected by the Control-Stake.
		uint8_t HarvestControlStakePercentage;
		

		/// Address of the harvest network fee sink account (V1).
		Address HarvestNetworkFeeSinkAddressPOS;

		/// Address of the harvest network fee sink account (latest).
		Address HarvestNetworkFeeSinkAddress;

		/// Address of the harvest Control-Stake fee sink account (V1).
		Address HarvestControlStakeFeeSinkAddressPOS;

		/// Address of the harvest Control-Stake fee sink account (latest).
		Address HarvestControlStakeFeeSinkAddress;

		/// Maximum number of transactions per block.
		uint32_t MaxTransactionsPerBlock;

	public:
		/// Fork heights configuration.
		struct ForkHeights {
			/// Height of fork to fix TotalVotingBalance calculation.
			Height TotalVotingBalanceCalculationFix;

			/// Height of fork at which to reissue the treasury.
			Height TreasuryReissuance;
		};

		/// Fork heights.
		BlockchainConfiguration::ForkHeights ForkHeights;

	public:
		/// Signatures of transactions allowed in the treasury reissuance block (preferred).
		/// \note These are allowed to involve the genesis account after the genesis block.
		std::vector<Signature> TreasuryReissuanceTransactionSignatures;

		/// Unparsed map of plugin configuration.
		std::unordered_map<std::string, utils::ConfigurationBag> Plugins;

	private:
		BlockchainConfiguration() = default;

	public:
		/// Creates an uninitialized blockchain configuration.
		static BlockchainConfiguration Uninitialized();

		/// Loads a blockchain configuration from \a bag.
		static BlockchainConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};

	/// Gets the unresolved currency token id from \a config.
	UnresolvedTokenId GetUnresolvedCurrencyTokenId(const BlockchainConfiguration& config);

	/// Gets the harvest network fee sink address from \a config.
	HeightDependentAddress GetHarvestNetworkFeeSinkAddress(const BlockchainConfiguration& config);

	/// Gets the harvest network fee sink address from \a config.
	HeightDependentAddress GetHarvestControlStakeFeeSinkAddress(const BlockchainConfiguration& config);

	/// Calculates the duration of time that expired transactions should be cached for the blockchain described by \a config.
	utils::TimeSpan CalculateTransactionCacheDuration(const BlockchainConfiguration& config);

	/// Loads plugin configuration for plugin named \a pluginName from \a config.
	template<typename T>
	T LoadPluginConfiguration(const BlockchainConfiguration& config, const std::string& pluginName) {
		auto iter = config.Plugins.find(pluginName);
		if (config.Plugins.cend() == iter)
			BITXORCORE_THROW_AND_LOG_1(utils::property_not_found_error, "plugin configuration not found", pluginName);

		return T::LoadFromBag(iter->second);
	}
}}
