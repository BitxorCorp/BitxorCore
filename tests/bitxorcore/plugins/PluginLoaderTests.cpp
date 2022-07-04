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

#include "bitxorcore/plugins/PluginLoader.h"
#include "bitxorcore/plugins/PluginExceptions.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/utils/ExceptionLogging.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/test/plugins/PluginManagerFactory.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace plugins {

#define TEST_CLASS PluginLoaderTests

	namespace {
		constexpr auto Known_Plugin_Name = "bitxorcore.plugins.transfer";

		void AssertCanLoadPlugins(
				const std::string& directory,
				const model::BlockchainConfiguration& config,
				bool isDynamicModule,
				const std::vector<std::string>& pluginNames) {
			// Arrange: ensure module is destroyed after manager
			for (const auto& name : pluginNames) {
				PluginModules modules;
				auto manager = test::CreatePluginManager(config);
				BITXORCORE_LOG(debug) << "loading plugin with name: " << name;

				// Act:
				LoadPluginByName(manager, modules, directory, name);

				// Assert: all known plugins have at least one observer or (stateless) validator
				EXPECT_FALSE(manager.createObserver()->names().empty() && manager.createStatelessValidator()->names().empty());

				// - check the module
				ASSERT_EQ(1u, modules.size());
				EXPECT_EQ(isDynamicModule, modules.back().isLoaded());
			}
		}

		void AssertCanLoadKnownDynamicallyLinkedPlugins(const std::string& directory) {
			// Arrange:
			auto config = model::BlockchainConfiguration::Uninitialized();
			config.Plugins.emplace(Known_Plugin_Name, utils::ConfigurationBag({{ "", { { "maxMessageSize", "0" } } }}));

			// Assert:
			AssertCanLoadPlugins(directory, config, true, { Known_Plugin_Name });
		}

		void AssertCannotLoadUnknownPlugin(const std::string& directory) {
			// Arrange:
			PluginModules modules;
			auto manager = test::CreatePluginManager();

			// Act + Assert:
			EXPECT_THROW(LoadPluginByName(manager, modules, directory, "bitxorcore.plugins.awesome"), bitxorcore_invalid_argument);
		}
	}

	TEST(TEST_CLASS, CanLoadKnownDynamicallyLinkedPlugins_ExplicitDirectory) {
		AssertCanLoadKnownDynamicallyLinkedPlugins(test::GetExplicitPluginsDirectory());
	}

	TEST(TEST_CLASS, CanLoadKnownDynamicallyLinkedPlugins_ImplicitDirectory) {
		AssertCanLoadKnownDynamicallyLinkedPlugins("");
	}

	TEST(TEST_CLASS, CannotLoadUnknownPlugin_ExplicitDirectory) {
		AssertCannotLoadUnknownPlugin(test::GetExplicitPluginsDirectory());
	}

	TEST(TEST_CLASS, CannotLoadUnknownPlugin_ImplicitDirectory) {
		AssertCannotLoadUnknownPlugin("");
	}

	TEST(TEST_CLASS, PluginRegistrationExceptionIsForwarded) {
		// Arrange:
		bool isExceptionHandled = false;
		try {
			// - prepare insufficient configuration
			auto config = model::BlockchainConfiguration::Uninitialized();
			config.Plugins.emplace(Known_Plugin_Name, utils::ConfigurationBag({{ "", { { "maxMessageSizeX", "0" } } }}));

			// - create the manager
			PluginModules modules;
			auto manager = test::CreatePluginManager(config);

			// Act:
			LoadPluginByName(manager, modules, "", Known_Plugin_Name);
		} catch (const plugin_load_error&) {
			BITXORCORE_LOG(debug) << UNHANDLED_EXCEPTION_MESSAGE("while running test");
			isExceptionHandled = true;
		}

		// Assert:
		EXPECT_TRUE(isExceptionHandled);
	}
}}
