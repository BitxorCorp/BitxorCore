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

#include "ValidateConfiguration.h"
#include "BitxorCoreConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/HexParser.h"

namespace bitxorcore { namespace config {

#define BITXORCORE_THROW_VALIDATION_ERROR(MESSAGE) BITXORCORE_THROW_AND_LOG_0(utils::property_malformed_error, MESSAGE)

	namespace {
		void ValidateConfiguration(const model::BlockchainConfiguration& config) {
			if (2 * config.ImportanceGrouping <= config.MaxRollbackBlocks) {
				std::ostringstream out;
				out
						<< "ImportanceGrouping (" << config.ImportanceGrouping << ") must be greater than MaxRollbackBlocks ("
						<< config.MaxRollbackBlocks << ") / 2";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}

			if (100u < uint(config.HarvestBeneficiaryPercentage + config.HarvestNetworkPercentage + config.HarvestControlStakePercentage)) {
				std::ostringstream out;
				out
						<< "HarvestBeneficiaryPercentage (" << utils::make_printable(config.HarvestBeneficiaryPercentage)
						<< ") plus HarvestNetworkPercentage (" << utils::make_printable(config.HarvestNetworkPercentage)
						<< ") plus HarvestControlStakePercentage (" << utils::make_printable(config.HarvestControlStakePercentage)
						<< ") must not be greater than 100";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}

			if (99u < config.ImportanceActivityPercentage) {
				std::ostringstream out;
				out
						<< "ImportanceActivityPercentage (" << utils::make_printable(config.ImportanceActivityPercentage)
						<< ") must not be greater than 99";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}

			if (0 != config.VotingSetGrouping % config.ImportanceGrouping) {
				std::ostringstream out;
				out
						<< "VotingSetGrouping (" << config.VotingSetGrouping << ") must be multiple of ImportanceGrouping ("
						<< config.ImportanceGrouping << ")";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}
		}

		void ValidateConfiguration(
				const model::BlockchainConfiguration& blockchainConfig,
				const config::InflationConfiguration& inflationConfig) {
			auto totalInflation = inflationConfig.InflationCalculator.sumAll();
			if (!totalInflation.second)
				BITXORCORE_THROW_VALIDATION_ERROR("total currency inflation could not be calculated");

			auto totalCurrency = blockchainConfig.InitialCurrencyAtomicUnits + totalInflation.first;
			if (blockchainConfig.InitialCurrencyAtomicUnits > totalCurrency || totalCurrency > blockchainConfig.MaxTokenAtomicUnits) {
				std::ostringstream out;
				out
						<< "sum of InitialCurrencyAtomicUnits (" << blockchainConfig.InitialCurrencyAtomicUnits << ") and inflation ("
						<< totalInflation.first << ") must not exceed MaxTokenAtomicUnits ("
						<< blockchainConfig.MaxTokenAtomicUnits << ")";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}
		}

		void ValidateConfiguration(const config::NodeConfiguration& config) {
			auto maxWriteBatchSize = config.CacheDatabase.MaxWriteBatchSize;
			if (utils::FileSize() != maxWriteBatchSize && maxWriteBatchSize < utils::FileSize::FromKilobytes(100)) {
				std::ostringstream out;
				out << "MaxWriteBatchSize (" << maxWriteBatchSize << ") must be unset or at least 100KB";
				BITXORCORE_THROW_VALIDATION_ERROR(out.str().c_str());
			}
		}
	}

	void ValidateConfiguration(const BitxorCoreConfiguration& config) {
		ValidateConfiguration(config.Blockchain);
		ValidateConfiguration(config.Blockchain, config.Inflation);
		ValidateConfiguration(config.Node);
	}

#undef BITXORCORE_THROW_VALIDATION_ERROR
}}
