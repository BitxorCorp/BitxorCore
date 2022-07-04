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

#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/utils/ConfigurationUtils.h"
#include "bitxorcore/utils/HexParser.h"
#include "tests/test/nodeps/ConfigurationTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS BlockchainConfigurationTests

	// region BlockchainConfiguration

	namespace {
		constexpr auto Genesis_Signer_Public_Key = "C738E237C98760FA72726BA13DC2A1E3C13FA67DE26AF09742E972EE4EE45E1C";
		constexpr auto Genesis_Generation_Hash_Seed = "CE076EF4ABFBC65B046987429E274EC31506D173E91BF102F16BEB7FB8176230";
		constexpr auto Harvest_Network_Fee_Sink_Address_V1 = "TESIA3QAZGTL56XLW3PK4RKDFP3VRHMML2HIGGY";
		constexpr auto Harvest_Network_Fee_Sink_Address = "TESO3ZTEVYPBXEOK2KVHXCUZTICYUWL4Y5FA6YI";

		constexpr auto Signature_1 =
				"395C2B37C7AABBEC3C08BD42DAF52D93D1BF003FF6A731E54F63003383EF1CE0"
				"302871ADD90DF04638DC617ACF2F5BB759C3DDC060E55A554477543210976C75";

		constexpr auto Signature_2 =
				"401ECCE607FF9710A00B677A487D36B9B9B3B0DC6DF59DA0A2BD77603E80B82B"
				"D0A82FE949055C5BB7A00F83AF4FF1242965CBF62C9D083344FF294D157259B2";

		constexpr auto Signature_3 =
				"3A785A34EA7FAB8AD7ED1B95EC0C0C1CC4097104DD3A47AB06E138D59DC48D75"
				"300996EDEF0C24641EE5EFFD83A3EFE10CE4CA41DAAF642342E988A0A0EA7FB6";

		struct BlockchainConfigurationTraits {
			using ConfigurationType = BlockchainConfiguration;

			static utils::ConfigurationBag::ValuesContainer CreateProperties() {
				return {
					{
						"network",
						{
							{ "identifier", "testnet" },
							{ "nodeEqualityStrategy", "host" },
							{ "genesisSignerPublicKey", Genesis_Signer_Public_Key },
							{ "generationHashSeed", Genesis_Generation_Hash_Seed },
							{ "epochAdjustment", "1234567h" }
						}
					},
					{
						"chain",
						{
							{ "enableVerifiableState", "true" },
							{ "enableVerifiableReceipts", "true" },

							{ "currencyTokenId", "0x1234'AAAA" },
							{ "harvestingTokenId", "0x9876'BBBB" },

							{ "blockGenerationTargetTime", "10m" },
							{ "blockTimeSmoothingFactor", "765" },

							{ "importanceGrouping", "444" },
							{ "importanceActivityPercentage", "15" },
							{ "maxRollbackBlocks", "720" },
							{ "maxDifficultyBlocks", "15" },
							{ "defaultDynamicFeeMultiplier", "9876" },

							{ "maxTransactionLifetime", "30m" },
							{ "maxBlockFutureTime", "21m" },

							{ "initialCurrencyAtomicUnits", "77'000'000'000" },
							{ "maxTokenAtomicUnits", "66'000'000'000" },

							{ "totalChainImportance", "88'000'000'000" },
							{ "minHarvesterBalance", "4'000'000'000" },
							{ "maxHarvesterBalance", "9'000'000'000" },
							{ "minVoterBalance", "2'000'000'000" },

							{ "votingSetGrouping", "234" },
							{ "maxVotingKeysPerAccount", "36" },
							{ "minVotingKeyLifetime", "21" },
							{ "maxVotingKeyLifetime", "123" },

							{ "harvestBeneficiaryPercentage", "56" },
							{ "harvestNetworkPercentage", "21" },
							{ "harvestNetworkFeeSinkAddressPOS", Harvest_Network_Fee_Sink_Address_V1 },
							{ "harvestNetworkFeeSinkAddress", Harvest_Network_Fee_Sink_Address },

							{ "maxTransactionsPerBlock", "120" }
						}
					},
					{
						"fork_heights",
						{
							{ "totalVotingBalanceCalculationFix", "998877" },
							{ "treasuryReissuance", "11998877" }
						}
					},
					{
						"treasury_reissuance_transaction_signatures",
						{
							{ Signature_1, "true" },
							{ Signature_2, "false" },
							{ Signature_3, "true" }
						}
					},
					{
						"plugin:alpha",
						{
							{ "foo", "alpha" }
						}
					},
					{
						"plugin:beta",
						{
							{ "bar", "11" },
							{ "baz", "zeta" }
						}
					}
				};
			}

			static bool IsSectionOptional(const std::string& section) {
				return "network" != section && "chain" != section;
			}

			static void AssertZero(const BlockchainConfiguration& config) {
				// Assert:
				EXPECT_EQ(NetworkIdentifier::Zero, config.Network.Identifier);
				EXPECT_EQ(static_cast<NodeIdentityEqualityStrategy>(0), config.Network.NodeEqualityStrategy);
				EXPECT_EQ(Key(), config.Network.GenesisSignerPublicKey);
				EXPECT_EQ(GenerationHashSeed(), config.Network.GenerationHashSeed);
				EXPECT_EQ(utils::TimeSpan(), config.Network.EpochAdjustment);

				EXPECT_FALSE(config.EnableVerifiableState);
				EXPECT_FALSE(config.EnableVerifiableReceipts);

				EXPECT_EQ(TokenId(), config.CurrencyTokenId);
				EXPECT_EQ(TokenId(), config.HarvestingTokenId);

				EXPECT_EQ(utils::TimeSpan::FromMinutes(0), config.BlockGenerationTargetTime);
				EXPECT_EQ(0u, config.BlockTimeSmoothingFactor);

				EXPECT_EQ(0u, config.ImportanceGrouping);
				EXPECT_EQ(0u, config.ImportanceActivityPercentage);
				EXPECT_EQ(0u, config.MaxRollbackBlocks);
				EXPECT_EQ(0u, config.MaxDifficultyBlocks);
				EXPECT_EQ(BlockFeeMultiplier(0), config.DefaultDynamicFeeMultiplier);

				EXPECT_EQ(utils::TimeSpan::FromMinutes(0), config.MaxTransactionLifetime);
				EXPECT_EQ(utils::TimeSpan::FromMinutes(0), config.MaxBlockFutureTime);

				EXPECT_EQ(Amount(0), config.InitialCurrencyAtomicUnits);
				EXPECT_EQ(Amount(0), config.MaxTokenAtomicUnits);

				EXPECT_EQ(Importance(0), config.TotalChainImportance);
				EXPECT_EQ(Amount(0), config.MinHarvesterBalance);
				EXPECT_EQ(Amount(0), config.MaxHarvesterBalance);
				EXPECT_EQ(Amount(0), config.MinVoterBalance);

				EXPECT_EQ(0u, config.VotingSetGrouping);
				EXPECT_EQ(0u, config.MaxVotingKeysPerAccount);
				EXPECT_EQ(0u, config.MinVotingKeyLifetime);
				EXPECT_EQ(0u, config.MaxVotingKeyLifetime);

				EXPECT_EQ(0u, config.HarvestBeneficiaryPercentage);
				EXPECT_EQ(0u, config.HarvestNetworkPercentage);
				EXPECT_EQ(Address(), config.HarvestNetworkFeeSinkAddressPOS);
				EXPECT_EQ(Address(), config.HarvestNetworkFeeSinkAddress);

				EXPECT_EQ(0u, config.MaxTransactionsPerBlock);

				EXPECT_EQ(Height(0), config.ForkHeights.TotalVotingBalanceCalculationFix);
				EXPECT_EQ(Height(0), config.ForkHeights.TreasuryReissuance);

				EXPECT_TRUE(config.TreasuryReissuanceTransactionSignatures.empty());
				EXPECT_TRUE(config.Plugins.empty());
			}

			static void AssertCustom(const BlockchainConfiguration& config) {
				// Assert: notice that ParseKey also works for Hash256 because it is the same type as Key
				EXPECT_EQ(NetworkIdentifier::Testnet, config.Network.Identifier);
				EXPECT_EQ(NodeIdentityEqualityStrategy::Host, config.Network.NodeEqualityStrategy);
				EXPECT_EQ(utils::ParseByteArray<Key>(Genesis_Signer_Public_Key), config.Network.GenesisSignerPublicKey);
				EXPECT_EQ(utils::ParseByteArray<GenerationHashSeed>(Genesis_Generation_Hash_Seed), config.Network.GenerationHashSeed);
				EXPECT_EQ(utils::TimeSpan::FromHours(1234567), config.Network.EpochAdjustment);

				EXPECT_TRUE(config.EnableVerifiableState);
				EXPECT_TRUE(config.EnableVerifiableReceipts);

				EXPECT_EQ(TokenId(0x1234'AAAA), config.CurrencyTokenId);
				EXPECT_EQ(TokenId(0x9876'BBBB), config.HarvestingTokenId);

				EXPECT_EQ(utils::TimeSpan::FromMinutes(10), config.BlockGenerationTargetTime);
				EXPECT_EQ(765u, config.BlockTimeSmoothingFactor);

				EXPECT_EQ(444u, config.ImportanceGrouping);
				EXPECT_EQ(15u, config.ImportanceActivityPercentage);
				EXPECT_EQ(720u, config.MaxRollbackBlocks);
				EXPECT_EQ(15u, config.MaxDifficultyBlocks);
				EXPECT_EQ(BlockFeeMultiplier(9876), config.DefaultDynamicFeeMultiplier);

				EXPECT_EQ(utils::TimeSpan::FromMinutes(30), config.MaxTransactionLifetime);
				EXPECT_EQ(utils::TimeSpan::FromMinutes(21), config.MaxBlockFutureTime);

				EXPECT_EQ(Amount(77'000'000'000), config.InitialCurrencyAtomicUnits);
				EXPECT_EQ(Amount(66'000'000'000), config.MaxTokenAtomicUnits);

				EXPECT_EQ(Importance(88'000'000'000), config.TotalChainImportance);
				EXPECT_EQ(Amount(4'000'000'000), config.MinHarvesterBalance);
				EXPECT_EQ(Amount(9'000'000'000), config.MaxHarvesterBalance);
				EXPECT_EQ(Amount(2'000'000'000), config.MinVoterBalance);

				EXPECT_EQ(234u, config.VotingSetGrouping);
				EXPECT_EQ(36u, config.MaxVotingKeysPerAccount);
				EXPECT_EQ(21u, config.MinVotingKeyLifetime);
				EXPECT_EQ(123u, config.MaxVotingKeyLifetime);

				EXPECT_EQ(56u, config.HarvestBeneficiaryPercentage);
				EXPECT_EQ(21, config.HarvestNetworkPercentage);
				EXPECT_EQ(StringToAddress(Harvest_Network_Fee_Sink_Address_V1), config.HarvestNetworkFeeSinkAddressPOS);
				EXPECT_EQ(StringToAddress(Harvest_Network_Fee_Sink_Address), config.HarvestNetworkFeeSinkAddress);

				EXPECT_EQ(120u, config.MaxTransactionsPerBlock);

				EXPECT_EQ(Height(998877), config.ForkHeights.TotalVotingBalanceCalculationFix);
				EXPECT_EQ(Height(11998877), config.ForkHeights.TreasuryReissuance);

				EXPECT_EQ(
						std::vector<Signature>({
							utils::ParseByteArray<Signature>(Signature_1),
							utils::ParseByteArray<Signature>(Signature_3)
						}),
						config.TreasuryReissuanceTransactionSignatures);

				EXPECT_EQ(2u, config.Plugins.size());
				const auto& pluginAlphaBag = config.Plugins.find("alpha")->second;
				EXPECT_EQ(1u, pluginAlphaBag.size());
				EXPECT_EQ("alpha", pluginAlphaBag.get<std::string>({ "", "foo" }));

				const auto& pluginBetaBag = config.Plugins.find("beta")->second;
				EXPECT_EQ(2u, pluginBetaBag.size());
				EXPECT_EQ(11u, pluginBetaBag.get<uint64_t>({ "", "bar" }));
				EXPECT_EQ("zeta", pluginBetaBag.get<std::string>({ "", "baz" }));
			}
		};
	}

	DEFINE_CONFIGURATION_TESTS(TEST_CLASS, Blockchain)

	TEST(TEST_CLASS, CannotLoadBlockchainConfigurationWithInvalidNetwork) {
		// Arrange: set an unknown network in the container
		using Traits = BlockchainConfigurationTraits;
		auto container = Traits::CreateProperties();
		auto& networkProperties = container["network"];
		auto hasIdentifierKey = [](const auto& pair) { return "identifier" == pair.first; };
		std::find_if(networkProperties.begin(), networkProperties.end(), hasIdentifierKey)->second = "foonet";

		// Act + Assert:
		EXPECT_THROW(Traits::ConfigurationType::LoadFromBag(std::move(container)), utils::property_malformed_error);
	}

	namespace {
		void AssertCannotLoadWithSection(const std::string& section) {
			// Arrange:
			using Traits = BlockchainConfigurationTraits;
			BITXORCORE_LOG(debug) << "attempting to load configuration with extra section '" << section << "'";

			// - create the properties and add the desired section
			auto properties = Traits::CreateProperties();
			properties.insert({ section, { { "foo", "1234" } } });

			// Act + Assert: the load failed
			EXPECT_THROW(Traits::ConfigurationType::LoadFromBag(std::move(properties)), bitxorcore_invalid_argument);
		}
	}

	TEST(TEST_CLASS, ParseFailsWhenPluginSectionNameIsNotWellFormed) {
		// Arrange: section name must start with 'plugin:' and have a name
		auto invalidSectionNames = { "", "plug", "plugina", "plug:a", "plugina:a", "a plugin:", "a plugin:b", "plugin:", " plugin:a" };
		for (const auto& section : invalidSectionNames)
			AssertCannotLoadWithSection(section);
	}

	TEST(TEST_CLASS, ParseFailsWhenPluginSectionNameContainsInvalidPluginName) {
		// Arrange:
		auto invalidPluginNames = { " ", "$$$", "some long name", "Zeta", "ZETA" };
		for (const auto& pluginName : invalidPluginNames)
			AssertCannotLoadWithSection(std::string("plugin:") + pluginName);
	}

	TEST(TEST_CLASS, ParseSucceedsWhenPluginSectionNameContainsValidPluginName) {
		// Arrange:
		using Traits = BlockchainConfigurationTraits;
		auto validPluginNames = { "a", "j", "z", ".", "zeta", "some.long.name" };
		for (const auto& pluginName : validPluginNames) {
			BITXORCORE_LOG(debug) << "attempting to load configuration with plugin named " << pluginName;

			// Act: create the properties and add the desired section
			auto properties = Traits::CreateProperties();
			properties.insert({ std::string("plugin:") + pluginName, { { "foo", "1234" } } });
			auto config = Traits::ConfigurationType::LoadFromBag(std::move(properties));

			// Assert:
			EXPECT_EQ(3u, config.Plugins.size());

			const auto& pluginBag = config.Plugins.find(pluginName)->second;
			EXPECT_EQ(1u, pluginBag.size());
			EXPECT_EQ(1234u, pluginBag.get<uint64_t>({ "", "foo" }));
		}
	}

	// endregion

	// region calculated properties

	TEST(TEST_CLASS, CanGetUnresolvedCurrencyTokenId) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.CurrencyTokenId = TokenId(1234);

		// Act + Assert:
		EXPECT_EQ(UnresolvedTokenId(1234), GetUnresolvedCurrencyTokenId(config));
	}

	TEST(TEST_CLASS, CanGetHarvestNetworkFeeSinkAddressWithoutFork) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.HarvestNetworkFeeSinkAddressPOS = test::GenerateRandomByteArray<Address>();
		config.HarvestNetworkFeeSinkAddress = test::GenerateRandomByteArray<Address>();
		config.ForkHeights.TreasuryReissuance = Height();

		// Act:
		auto sinkAddress = GetHarvestNetworkFeeSinkAddress(config);

		// Assert:
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddress, sinkAddress.get(Height(0)));
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddress, sinkAddress.get(Height(1)));
	}

	TEST(TEST_CLASS, CanGetHarvestNetworkFeeSinkAddressWithFork) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.HarvestNetworkFeeSinkAddressPOS = test::GenerateRandomByteArray<Address>();
		config.HarvestNetworkFeeSinkAddress = test::GenerateRandomByteArray<Address>();
		config.ForkHeights.TreasuryReissuance = Height(1234);

		// Act:
		auto sinkAddress = GetHarvestNetworkFeeSinkAddress(config);

		// Assert:
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddress, sinkAddress.get(Height(0)));
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddressPOS, sinkAddress.get(Height(1)));

		EXPECT_EQ(config.HarvestNetworkFeeSinkAddressPOS, sinkAddress.get(Height(1233)));
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddress, sinkAddress.get(Height(1234)));
		EXPECT_EQ(config.HarvestNetworkFeeSinkAddress, sinkAddress.get(Height(1235)));
	}

	namespace {
		const uint64_t One_Hour_Ms = []() { return utils::TimeSpan::FromHours(1).millis(); }();

		utils::TimeSpan TimeSpanFromMillis(uint64_t millis) {
			return utils::TimeSpan::FromMilliseconds(millis);
		}
	}

	TEST(TEST_CLASS, CanCalculateDependentSettingsFromCustomBlockchainConfiguration) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.BlockGenerationTargetTime = TimeSpanFromMillis(30'001);
		config.MaxTransactionLifetime = TimeSpanFromMillis(One_Hour_Ms - 1);
		config.MaxRollbackBlocks = 600;

		// Act + Assert:
		EXPECT_EQ(TimeSpanFromMillis(30'001 * (600 + 150)), CalculateTransactionCacheDuration(config));
	}

	TEST(TEST_CLASS, CanCalculateDependentSettingsFromCustomBlockchainConfiguration_MaxRollbackBlocksZero) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.BlockGenerationTargetTime = TimeSpanFromMillis(30'001);
		config.MaxTransactionLifetime = TimeSpanFromMillis(One_Hour_Ms - 1);
		config.MaxRollbackBlocks = 0;

		// Act + Assert:
		EXPECT_EQ(TimeSpanFromMillis(One_Hour_Ms - 1), CalculateTransactionCacheDuration(config));
	}

	TEST(TEST_CLASS, TransactionCacheDurationIncludesBufferTimeOfAtLeastOneHour) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.BlockGenerationTargetTime = utils::TimeSpan::FromSeconds(15);
		config.MaxTransactionLifetime = utils::TimeSpan::FromHours(2);
		config.MaxRollbackBlocks = 20;

		// Act + Assert:
		EXPECT_EQ(TimeSpanFromMillis(15'000 * 20 + One_Hour_Ms), CalculateTransactionCacheDuration(config));
	}

	// endregion

	// region LoadPluginConfiguration

	namespace {
		struct BetaConfiguration {
		public:
			uint64_t Bar;
			std::string Baz;

		public:
			static BetaConfiguration LoadFromBag(const utils::ConfigurationBag& bag) {
				BetaConfiguration config;
				utils::LoadIniProperty(bag, "", "Bar", config.Bar);
				utils::LoadIniProperty(bag, "", "Baz", config.Baz);
				utils::VerifyBagSizeExact(bag, 2);
				return config;
			}
		};
	}

	TEST(TEST_CLASS, LoadPluginConfigurationSucceedsWhenPluginConfigurationIsPresent) {
		// Arrange:
		using Traits = BlockchainConfigurationTraits;
		auto config = Traits::ConfigurationType::LoadFromBag(Traits::CreateProperties());

		// Act:
		auto betaConfig = LoadPluginConfiguration<BetaConfiguration>(config, "beta");

		// Assert:
		EXPECT_EQ(11u, betaConfig.Bar);
		EXPECT_EQ("zeta", betaConfig.Baz);
	}

	TEST(TEST_CLASS, LoadPluginConfigurationFailsWhenPluginConfigurationIsNotPresent) {
		// Arrange:
		using Traits = BlockchainConfigurationTraits;
		auto config = Traits::ConfigurationType::LoadFromBag(Traits::CreateProperties());

		// Act + Assert:
		EXPECT_THROW(LoadPluginConfiguration<BetaConfiguration>(config, "gamma"), utils::property_not_found_error);
	}

	// endregion
}}
