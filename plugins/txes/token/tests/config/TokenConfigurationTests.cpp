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

#include "src/config/TokenConfiguration.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/utils/HexParser.h"
#include "tests/test/nodeps/ConfigurationTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace config {

#define TEST_CLASS TokenConfigurationTests

	// region TokenConfiguration

	namespace {
		constexpr auto Token_Rental_Fee_Sink_Address_V1 = "TESPKWNIHCQRAAC2HRUSUFTQQJY4JA2UFR6YEOQ";
		constexpr auto Token_Rental_Fee_Sink_Address = "TESPKWNIHCQRAAC2HRUSUFTQQJY4JA2UFR6YEOQ";

		struct TokenConfigurationTraits {
			using ConfigurationType = TokenConfiguration;

			static utils::ConfigurationBag::ValuesContainer CreateProperties() {
				return {
					{
						"",
						{
							{ "maxTokensPerAccount", "4321" },
							{ "maxTokenDuration", "2340h" },
							{ "maxTokenDivisibility", "7" },

							{ "tokenRentalFeeSinkAddressPOS", Token_Rental_Fee_Sink_Address_V1 },
							{ "tokenRentalFeeSinkAddress", Token_Rental_Fee_Sink_Address },
							{ "tokenRentalFee", "773388" }
						}
					}
				};
			}

			static bool IsSectionOptional(const std::string&) {
				return false;
			}

			static void AssertZero(const TokenConfiguration& config) {
				// Assert:
				EXPECT_EQ(0u, config.MaxTokensPerAccount);
				EXPECT_EQ(utils::BlockSpan(), config.MaxTokenDuration);
				EXPECT_EQ(0u, config.MaxTokenDivisibility);

				EXPECT_EQ(Address(), config.TokenRentalFeeSinkAddressPOS);
				EXPECT_EQ(Address(), config.TokenRentalFeeSinkAddress);
				EXPECT_EQ(Amount(), config.TokenRentalFee);
			}

			static void AssertCustom(const TokenConfiguration& config) {
				// Assert:
				EXPECT_EQ(4321u, config.MaxTokensPerAccount);
				EXPECT_EQ(utils::BlockSpan::FromHours(2340), config.MaxTokenDuration);
				EXPECT_EQ(7u, config.MaxTokenDivisibility);

				EXPECT_EQ(model::StringToAddress(Token_Rental_Fee_Sink_Address_V1), config.TokenRentalFeeSinkAddressPOS);
				EXPECT_EQ(model::StringToAddress(Token_Rental_Fee_Sink_Address), config.TokenRentalFeeSinkAddress);
				EXPECT_EQ(Amount(773388), config.TokenRentalFee);
			}
		};
	}

	DEFINE_CONFIGURATION_TESTS(TokenConfigurationTests, Token)

	// endregion

	// region calculated properties

	TEST(TEST_CLASS, CanGetTokenRentalFeeSinkAddressWithoutFork) {
		// Arrange:
		auto blockchainConfig = model::BlockchainConfiguration::Uninitialized();
		blockchainConfig.ForkHeights.TreasuryReissuance = Height();

		auto config = TokenConfiguration::Uninitialized();
		config.TokenRentalFeeSinkAddressPOS = test::GenerateRandomByteArray<Address>();
		config.TokenRentalFeeSinkAddress = test::GenerateRandomByteArray<Address>();

		// Act:
		auto sinkAddress = GetTokenRentalFeeSinkAddress(config, blockchainConfig);

		// Assert:
		EXPECT_EQ(config.TokenRentalFeeSinkAddress, sinkAddress.get(Height(0)));
		EXPECT_EQ(config.TokenRentalFeeSinkAddress, sinkAddress.get(Height(1)));
	}

	TEST(TEST_CLASS, CanGetTokenRentalFeeSinkAddressWithFork) {
		// Arrange:
		auto blockchainConfig = model::BlockchainConfiguration::Uninitialized();
		blockchainConfig.ForkHeights.TreasuryReissuance = Height(1234);

		auto config = TokenConfiguration::Uninitialized();
		config.TokenRentalFeeSinkAddressPOS = test::GenerateRandomByteArray<Address>();
		config.TokenRentalFeeSinkAddress = test::GenerateRandomByteArray<Address>();

		// Act:
		auto sinkAddress = GetTokenRentalFeeSinkAddress(config, blockchainConfig);

		// Assert:
		EXPECT_EQ(config.TokenRentalFeeSinkAddress, sinkAddress.get(Height(0)));
		EXPECT_EQ(config.TokenRentalFeeSinkAddressPOS, sinkAddress.get(Height(1)));

		EXPECT_EQ(config.TokenRentalFeeSinkAddressPOS, sinkAddress.get(Height(1233)));
		EXPECT_EQ(config.TokenRentalFeeSinkAddress, sinkAddress.get(Height(1234)));
		EXPECT_EQ(config.TokenRentalFeeSinkAddress, sinkAddress.get(Height(1235)));
	}

	// endregion
}}
