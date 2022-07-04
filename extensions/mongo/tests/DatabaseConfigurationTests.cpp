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

#include "mongo/src/DatabaseConfiguration.h"
#include "tests/test/nodeps/ConfigurationTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo {

#define TEST_CLASS DatabaseConfigurationTests

	namespace {
		struct DatabaseConfigurationTraits {
			using ConfigurationType = DatabaseConfiguration;

			static utils::ConfigurationBag::ValuesContainer CreateProperties() {
				return {
					{
						"database",
						{
							{ "databaseUri", "mongodb://hostname:port" },
							{ "databaseName", "foo" },
							{ "maxWriterThreads", "3" },
							{ "maxDropBatchSize", "7" },
							{ "writeTimeout", "22s" }
						}
					},
					{
						"plugins",
						{
							{ "Alpha", "true" },
							{ "BETA", "false" },
							{ "gamma", "true" }
						}
					}
				};
			}

			static bool IsSectionOptional(const std::string& section) {
				return "plugins" == section;
			}

			static void AssertZero(const DatabaseConfiguration& config) {
				// Assert:
				EXPECT_EQ("", config.DatabaseUri);
				EXPECT_EQ("", config.DatabaseName);
				EXPECT_EQ(0u, config.MaxWriterThreads);
				EXPECT_EQ(0u, config.MaxDropBatchSize);
				EXPECT_EQ(utils::TimeSpan(), config.WriteTimeout);
				EXPECT_EQ(std::unordered_set<std::string>(), config.Plugins);
			}

			static void AssertCustom(const DatabaseConfiguration& config) {
				// Assert:
				EXPECT_EQ("mongodb://hostname:port", config.DatabaseUri);
				EXPECT_EQ("foo", config.DatabaseName);
				EXPECT_EQ(3u, config.MaxWriterThreads);
				EXPECT_EQ(7u, config.MaxDropBatchSize);
				EXPECT_EQ(utils::TimeSpan::FromSeconds(22), config.WriteTimeout);
				EXPECT_EQ(std::unordered_set<std::string>({ "Alpha", "gamma" }), config.Plugins);
			}
		};
	}

	DEFINE_CONFIGURATION_TESTS(TEST_CLASS, Database)

	// region file io

	TEST(TEST_CLASS, LoadFromPathFailsWhenFileDoesNotExist) {
		// Act + Assert: attempt to load the config
		EXPECT_THROW(DatabaseConfiguration::LoadFromPath("../no-resources"), bitxorcore_runtime_error);
	}

	TEST(TEST_CLASS, CanLoadConfigFromResourcesDirectory) {
		// Act: attempt to load from the "real" resources directory
		auto config = DatabaseConfiguration::LoadFromPath("../resources");

		// Assert:
		EXPECT_EQ("mongodb://127.0.0.1:27017", config.DatabaseUri);
		EXPECT_EQ("bitxorcore", config.DatabaseName);
		EXPECT_EQ(8u, config.MaxWriterThreads);
		EXPECT_EQ(100u, config.MaxDropBatchSize);
		EXPECT_EQ(utils::TimeSpan::FromMinutes(10), config.WriteTimeout);
		EXPECT_FALSE(config.Plugins.empty());
	}

	// endregion
}}
