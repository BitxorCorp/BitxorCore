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

#include "tools/ToolMain.h"
#include "AdditionalTransactions.h"
#include "BlockGenerator.h"
#include "BlockSaver.h"
#include "GenesisConfigurationLoader.h"
#include "GenesisExecutionHasher.h"
#include "tools/plugins/PluginLoader.h"
#include "tools/ToolConfigurationUtils.h"
#include "bitxorcore/io/RawFile.h"

namespace bitxorcore { namespace tools { namespace bxorgen {

	namespace {
		void WriteToFile(const std::string& filePath, const std::string& content) {
			io::RawFile file(filePath, io::OpenMode::Read_Write);
			file.write({ reinterpret_cast<const uint8_t*>(content.data()), content.size() });
		}

		bool IsExtensionEnabled(const config::ExtensionsConfiguration& extensionsConfig, const std::string& name) {
			const auto& names = extensionsConfig.Names;
			return names.cend() != std::find(names.cbegin(), names.cend(), name);
		}

		class BxorGenTool : public Tool {
		public:
			std::string name() const override {
				return "Genesis Block Generator Tool";
			}

			void prepareOptions(OptionsBuilder& optionsBuilder, OptionsPositional&) override {
				AddResourcesOption(optionsBuilder);

				optionsBuilder("genesisProperties,p",
						OptionsValue<std::string>(m_genesisPropertiesFilePath),
						"path to the genesis properties file");

				optionsBuilder("summary,s",
						OptionsValue<std::string>(m_summaryFilePath),
						"path to summary output file (default: <bindir>/summary.txt)");

				optionsBuilder("no-summary,n",
						OptionsSwitch(),
						"don't generate summary file");

				optionsBuilder("useTemporaryCacheDatabase,t",
						OptionsSwitch(),
						"true if a temporary cache database should be created and destroyed");
			}

			int run(const Options& options) override {
				// 1. load config and disable loading of user certificates during block generation
				auto config = LoadConfiguration(GetResourcesOptionValue(options));
				const_cast<bool&>(config.User.EnableDelegatedHarvestersAutoDetection) = false;

				auto genesisConfig = LoadGenesisConfiguration(m_genesisPropertiesFilePath);
				if (!LogAndValidateGenesisConfiguration(genesisConfig))
					return -1;

				// 2. load transaction plugins
				auto databaseCleanupMode = options["useTemporaryCacheDatabase"].as<bool>()
						? plugins::CacheDatabaseCleanupMode::Purge
						: plugins::CacheDatabaseCleanupMode::None;
				plugins::PluginLoader pluginLoader(config, databaseCleanupMode);
				pluginLoader.loadAll();

				// 3. create the genesis block element
				auto additionalTransactions = LoadAndValidateAdditionalTransactions(
						genesisConfig,
						*pluginLoader.createNotificationPublisher());
				auto pBlock = CreateGenesisBlock(genesisConfig, std::move(additionalTransactions));
				auto blockElement = CreateGenesisBlockElement(genesisConfig, pluginLoader.transactionRegistry(), *pBlock);
				auto executionHashesDescriptor = CalculateAndLogGenesisExecutionHashes(blockElement, config, pluginLoader.manager());
				if (!options["no-summary"].as<bool>()) {
					if (m_summaryFilePath.empty())
						m_summaryFilePath = genesisConfig.BinDirectory + "/summary.txt";

					WriteToFile(m_summaryFilePath, executionHashesDescriptor.Summary);
				}

				// 4. update block with result of execution
				BITXORCORE_LOG(info) << "*** Genesis Summary ***" << std::endl << executionHashesDescriptor.Summary;
				blockElement.EntityHash = UpdateGenesisBlock(genesisConfig, *pBlock, executionHashesDescriptor);
				blockElement.SubCacheMerkleRoots = executionHashesDescriptor.SubCacheMerkleRoots;
				if (config.Blockchain.EnableVerifiableReceipts)
					blockElement.OptionalStatement = std::move(executionHashesDescriptor.pBlockStatement);

				// 5. save the genesis block element
				SaveGenesisBlockElement(blockElement, genesisConfig);

				if (IsExtensionEnabled(config.Extensions, "extension.finalization")) {
					BITXORCORE_LOG(info) << "finalizing genesis to storage";
					FinalizeGenesisBlockElement(blockElement, genesisConfig);
				}

				return 0;
			}

		private:
			std::string m_genesisPropertiesFilePath;
			std::string m_summaryFilePath;
		};
	}
}}}

int main(int argc, const char** argv) {
	bitxorcore::tools::bxorgen::BxorGenTool bxorGenTool;
	return bitxorcore::tools::ToolMain(argc, argv, bxorGenTool);
}
