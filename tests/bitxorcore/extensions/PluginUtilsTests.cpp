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

#include "bitxorcore/extensions/PluginUtils.h"
#include "plugins/coresystem/src/validators/Results.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/model/Transaction.h"
#include "tests/test/local/LocalTestUtils.h"
#include "tests/test/local/RealTransactionFactory.h"
#include "tests/test/other/MutableBitxorCoreConfiguration.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS PluginUtilsTests

	TEST(TEST_CLASS, CanCreateStorageConfiguration) {
		// Arrange:
		test::MutableBitxorCoreConfiguration config;
		config.Node.EnableCacheDatabaseStorage = true;
		config.Node.CacheDatabase.MaxWriteBatchSize = utils::FileSize::FromKilobytes(123);
		config.User.DataDirectory = "foo_bar";

		// Act:
		auto storageConfig = CreateStorageConfiguration(config.ToConst());

		// Assert:
		EXPECT_TRUE(storageConfig.PreferCacheDatabase);
		EXPECT_EQ("foo_bar/statedb", storageConfig.CacheDatabaseDirectory);
		EXPECT_EQ(utils::FileSize::FromKilobytes(123), storageConfig.CacheDatabaseConfig.MaxWriteBatchSize);
	}

	namespace {
		template<typename TFactory>
		void AssertCanCreateStatelessEntityValidator(validators::ValidationResult expectedValidationResult, TFactory factory) {
			// Arrange:
			auto config = model::BlockchainConfiguration::Uninitialized();
			config.Plugins.emplace("bitxorcore.plugins.transfer", utils::ConfigurationBag({{ "", { { "maxMessageSize", "0" } } }}));
			auto pPluginManager = test::CreatePluginManagerWithRealPlugins(config);

			// Act:
			auto pEntityValidator = factory(*pPluginManager);

			// Assert:
			ASSERT_TRUE(!!pEntityValidator);
			EXPECT_EQ(pPluginManager->createStatelessValidator()->name(), pEntityValidator->name());

			// - validate a real transaction as a proxy for testing notification type filtering
			auto pTransaction = test::CreateUnsignedTransferTransaction(
					test::GenerateRandomByteArray<Key>(),
					test::GenerateRandomByteArray<UnresolvedAddress>(),
					Amount(0));
			Hash256 transactionHash;
			auto result = pEntityValidator->validate({ *pTransaction, transactionHash });
			EXPECT_EQ(expectedValidationResult, result);
		}
	}

	TEST(TEST_CLASS, CanCreateStatelessEntityValidator) {
		AssertCanCreateStatelessEntityValidator(validators::Failure_Core_Wrong_Network, [](const auto& pluginManager) {
			return CreateStatelessEntityValidator(pluginManager);
		});
	}

	TEST(TEST_CLASS, CanCreateStatelessEntityValidatorWithNotificationTypeFilter) {
		AssertCanCreateStatelessEntityValidator(validators::Failure_Core_Invalid_Transaction_Fee, [](const auto& pluginManager) {
			// Act: suppress EntityNotification, which raises Failure_Core_Wrong_Network
			return CreateStatelessEntityValidator(pluginManager, model::EntityNotification::Notification_Type);
		});
	}

	TEST(TEST_CLASS, CanCreateUndoEntityObserver) {
		// Arrange:
		auto pPluginManager = test::CreateDefaultPluginManagerWithRealPlugins();

		// Act:
		auto pEntityObserver = CreateUndoEntityObserver(*pPluginManager);

		// Assert:
		ASSERT_TRUE(!!pEntityObserver);
		EXPECT_EQ(pPluginManager->createObserver()->name(), pEntityObserver->name());
	}
}}
