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

#include "GenesisConfigurationLoader.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/HexFormatter.h"
#include <filesystem>

namespace bitxorcore { namespace tools { namespace bxorgen {

	namespace {
		template<typename TIdentifier>
		void OutputName(const std::string& name, TIdentifier id) {
			BITXORCORE_LOG(debug) << " - " << name << " (" << utils::HexFormat(id) << ")";
		}

		void LogNamespaces(const GenesisConfiguration& config) {
			BITXORCORE_LOG(debug) << "Namespace Summary";
			for (const auto& pair : config.RootNamespaces) {
				const auto& root = pair.second;
				const auto& name = config.NamespaceNames.at(root.id());
				OutputName(name, root.id());
				BITXORCORE_LOG(debug) << " - Owner: " << root.ownerAddress();
				BITXORCORE_LOG(debug) << " - Start Height: " << root.lifetime().Start;
				BITXORCORE_LOG(debug) << " - End Height: " << root.lifetime().End;
				if (!root.empty()) {
					BITXORCORE_LOG(debug) << " - Children:";
					for (const auto & childPair : root.children()) {
						const auto& childName = config.NamespaceNames.at(childPair.first);
						BITXORCORE_LOG(debug) << " - - " << childName << " (" << utils::HexFormat(childPair.first) << ")";
					}
				}

				BITXORCORE_LOG(debug);
			}
		}

		bool LogTokenDefinitions(const GenesisConfiguration& config) {
			BITXORCORE_LOG(debug) << "Token Summary";
			std::unordered_set<TokenId, utils::BaseValueHasher<TokenId>> tokenIds;
			for (const auto& pair : config.TokenEntries) {
				const std::string& name = pair.first;
				auto id = pair.second.tokenId();
				const auto& tokenEntry = pair.second;
				const auto& definition = tokenEntry.definition();
				const auto& properties = definition.properties();
				OutputName(name, id);
				BITXORCORE_LOG(debug)
						<< " - Owner: " << definition.ownerAddress() << std::endl
						<< " - Supply: " << tokenEntry.supply() << std::endl
						<< " - Divisibility: " << static_cast<uint32_t>(properties.divisibility()) << std::endl
						<< " - Duration: " << properties.duration() << " blocks (0 = eternal)" << std::endl
						<< " - IsTransferable: " << properties.is(model::TokenFlags::Transferable) << std::endl
						<< " - IsSupplyMutable: " << properties.is(model::TokenFlags::Supply_Mutable) << std::endl
						<< " - IsRestrictable: " << properties.is(model::TokenFlags::Restrictable) << std::endl
						<< std::endl;

				if (!tokenIds.insert(id).second) {
					BITXORCORE_LOG(warning) << "token " << name << " does not have a unique id";
					return false;
				}
			}

			return true;
		}

		bool LogTokenDistribution(const GenesisConfiguration& config) {
			BITXORCORE_LOG(debug) << "Genesis Seed Amounts";
			for (const auto& addressTokenSeedsPair : config.GenesisAddressToTokenSeeds) {
				const auto& address = addressTokenSeedsPair.first;
				BITXORCORE_LOG(debug) << " - " << address;
				if (!model::IsValidEncodedAddress(address, config.NetworkIdentifier)) {
					BITXORCORE_LOG(warning) << "address " << address << " is invalid";
					return false;
				}

				for (const auto& seed : addressTokenSeedsPair.second)
					BITXORCORE_LOG(debug) << " - - " << seed.Name << ": " << seed.Amount;
			}

			return true;
		}
	}

	GenesisConfiguration LoadGenesisConfiguration(const std::string& configPath) {
		if (!std::filesystem::exists(configPath)) {
			auto message = "aborting load due to missing configuration file";
			BITXORCORE_LOG(fatal) << message << ": " << configPath;
			BITXORCORE_THROW_EXCEPTION(bitxorcore_runtime_error(message));
		}

		BITXORCORE_LOG(info) << "loading genesis configuration from " << configPath;
		return GenesisConfiguration::LoadFromBag(utils::ConfigurationBag::FromPath(configPath));
	}

	bool LogAndValidateGenesisConfiguration(const GenesisConfiguration& config) {
		BITXORCORE_LOG(debug) << "--- Genesis Configuration ---";
		BITXORCORE_LOG(debug) << "Network            : " << config.NetworkIdentifier;
		BITXORCORE_LOG(debug) << "Gen Hash Seed      : " << config.GenesisGenerationHashSeed;
		BITXORCORE_LOG(debug) << "Genesis Private Key: " << config.GenesisSignerPrivateKey;
		BITXORCORE_LOG(debug) << "Txes Directory     : " << config.TransactionsDirectory;
		BITXORCORE_LOG(debug) << "Cpp File Header    : " << config.CppFileHeader;
		BITXORCORE_LOG(debug) << "Cpp File           : " << config.CppFile;
		BITXORCORE_LOG(debug) << "Bin Directory      : " << config.BinDirectory;

		// - namespaces
		LogNamespaces(config);

		// - token definitions and distribution
		return LogTokenDefinitions(config) && LogTokenDistribution(config);
	}
}}}
