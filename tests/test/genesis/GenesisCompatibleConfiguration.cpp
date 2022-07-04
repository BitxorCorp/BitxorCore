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

#include "GenesisCompatibleConfiguration.h"
#include "tests/test/local/LocalTestUtils.h"
#include "tests/test/nodeps/TestNetworkConstants.h"

namespace bitxorcore { namespace test {

	namespace {
		model::BlockchainConfiguration CreateBlockchainConfiguration() {
			auto config = CreatePrototypicalBlockchainConfiguration();
			AddGenesisPluginExtensions(config);
			return config;
		}
	}

	void AddGenesisPluginExtensions(model::BlockchainConfiguration& config) {
		config.Plugins.emplace("bitxorcore.plugins.transfer", utils::ConfigurationBag({{ "", { { "maxMessageSize", "0" } } }}));
		config.Plugins.emplace("bitxorcore.plugins.token", utils::ConfigurationBag({{
			"",
			{
				{ "maxTokensPerAccount", "123" },
				{ "maxTokenDuration", "456d" },
				{ "maxTokenDivisibility", "6" },

				{ "tokenRentalFeeSinkAddressPOS", Token_Rental_Fee_Sink_Address },
				{ "tokenRentalFeeSinkAddress", Token_Rental_Fee_Sink_Address },
				{ "tokenRentalFee", "500" }
			}
		}}));
		config.Plugins.emplace("bitxorcore.plugins.namespace", utils::ConfigurationBag({{
			"",
			{
				{ "maxNameSize", "64" },
				{ "maxChildNamespaces", "100" },
				{ "maxNamespaceDepth", "3" },

				{ "minNamespaceDuration", "1m" },
				{ "maxNamespaceDuration", "365d" },
				{ "namespaceGracePeriodDuration", "1h" },
				{ "reservedRootNamespaceNames", "cat" },

				{ "namespaceRentalFeeSinkAddressPOS", Namespace_Rental_Fee_Sink_Address },
				{ "namespaceRentalFeeSinkAddress", Namespace_Rental_Fee_Sink_Address },
				{ "rootNamespaceRentalFeePerBlock", "10" },
				{ "childNamespaceRentalFee", "10000" }
			}
		}}));
	}

	namespace {
		void AddPluginExtensions(config::ExtensionsConfiguration& config, const std::unordered_set<std::string>& extensionNames) {
			for (const auto& extensionName : extensionNames)
				config.Names.emplace_back("extension." + extensionName);
		}

		void AddCommonPluginExtensions(config::ExtensionsConfiguration& config) {
			// finalization is needed because int tests are run with 0 == MaxRollbackBlocks
			AddPluginExtensions(config, { "diagnostics", "finalization", "packetserver", "sync", "transactionsink" });
		}
	}

	void AddApiPluginExtensions(config::ExtensionsConfiguration& config) {
		AddCommonPluginExtensions(config);
	}

	void AddPeerPluginExtensions(config::ExtensionsConfiguration& config) {
		AddCommonPluginExtensions(config);
		AddPluginExtensions(config, { "harvesting", "syncsource" });
	}

	void AddSimplePartnerPluginExtensions(config::ExtensionsConfiguration& config) {
		AddPluginExtensions(config, { "finalization", "packetserver", "sync", "syncsource" });
	}

	void AddRecoveryPluginExtensions(config::ExtensionsConfiguration& config) {
		AddPluginExtensions(config, {});
	}

	config::BitxorCoreConfiguration CreateBitxorCoreConfigurationWithGenesisPluginExtensions(const std::string& dataDirectory) {
		return CreatePrototypicalBitxorCoreConfiguration(CreateBlockchainConfiguration(), dataDirectory);
	}
}}
