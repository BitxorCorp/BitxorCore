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

#include "bitxorcore/config/UserConfiguration.h"
#include "tests/test/nodeps/ConfigurationTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace config {

	namespace {
		struct UserConfigurationTraits {
			using ConfigurationType = UserConfiguration;

			static utils::ConfigurationBag::ValuesContainer CreateProperties() {
				return {
					{
						"account",
						{
							{ "enableDelegatedHarvestersAutoDetection", "true" }
						}
					},
					{
						"storage",
						{
							{ "seedDirectory", "./sd" },
							{ "dataDirectory", "./dd" },
							{ "certificateDirectory", "./cert" },
							{ "votingKeysDirectory", "./keys" },
							{ "pluginsDirectory", "./ext" }
						}
					}
				};
			}

			static bool IsSectionOptional(const std::string&) {
				return false;
			}

			static void AssertZero(const UserConfiguration& config) {
				// Assert:
				EXPECT_FALSE(config.EnableDelegatedHarvestersAutoDetection);

				EXPECT_EQ("", config.SeedDirectory);
				EXPECT_EQ("", config.DataDirectory);
				EXPECT_EQ("", config.CertificateDirectory);
				EXPECT_EQ("", config.VotingKeysDirectory);
				EXPECT_EQ("", config.PluginsDirectory);
			}

			static void AssertCustom(const UserConfiguration& config) {
				// Assert:
				EXPECT_TRUE(config.EnableDelegatedHarvestersAutoDetection);

				EXPECT_EQ("./sd", config.SeedDirectory);
				EXPECT_EQ("./dd", config.DataDirectory);
				EXPECT_EQ("./cert", config.CertificateDirectory);
				EXPECT_EQ("./keys", config.VotingKeysDirectory);
				EXPECT_EQ("./ext", config.PluginsDirectory);
			}
		};
	}

	DEFINE_CONFIGURATION_TESTS(UserConfigurationTests, User)
}}
