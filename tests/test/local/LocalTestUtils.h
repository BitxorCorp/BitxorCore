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

#pragma once
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "tests/test/core/AddressTestUtils.h"
#include "tests/test/net/NodeTestUtils.h"
#include "tests/TestHarness.h"
#include <string>

namespace bitxorcore {
	namespace cache {
		class BitxorCoreCache;
		class MemoryUtCache;
		class MemoryUtCacheProxy;
		class MemoryUtCacheView;
		class UtCache;
	}
	namespace chain { class UtUpdater; }
}

namespace bitxorcore { namespace test {

	/// Bit flags for configuring a LocalNode under test.
	enum class LocalNodeFlags {
		/// No special configuration flag is set.
		None = 0,

		/// Local node should harvest upon startup.
		Should_Auto_Harvest = 2
	};

	/// Creates a default network time supplier for use in local tests.
	supplier<Timestamp> CreateDefaultNetworkTimeSupplier();

	/// Creates a prototypical blockchain configuration that is safe to use in local tests.
	model::BlockchainConfiguration CreatePrototypicalBlockchainConfiguration();

	/// Creates an uninitialized bitxorcore configuration.
	config::BitxorCoreConfiguration CreateUninitializedBitxorCoreConfiguration();

	/// Creates a prototypical bitxorcore configuration that is safe to use in local tests.
	config::BitxorCoreConfiguration CreatePrototypicalBitxorCoreConfiguration();

	/// Creates a test bitxorcore configuration with a storage in the specified directory (\a dataDirectory).
	config::BitxorCoreConfiguration CreatePrototypicalBitxorCoreConfiguration(const std::string& dataDirectory);

	/// Creates a test bitxorcore configuration according to the supplied configuration (\a blockchainConfig)
	/// with a storage in the specified directory (\a dataDirectory).
	config::BitxorCoreConfiguration CreatePrototypicalBitxorCoreConfiguration(
			model::BlockchainConfiguration&& blockchainConfig,
			const std::string& dataDirectory);

	/// Creates a default unconfirmed transactions cache.
	std::unique_ptr<cache::MemoryUtCache> CreateUtCache();

	/// Creates a default unconfirmed transactions cache proxy.
	std::unique_ptr<cache::MemoryUtCacheProxy> CreateUtCacheProxy();

	/// Creates a default plugin manager.
	std::shared_ptr<plugins::PluginManager> CreateDefaultPluginManagerWithRealPlugins();

	/// Creates a plugin manager around \a config.
	std::shared_ptr<plugins::PluginManager> CreatePluginManagerWithRealPlugins(const model::BlockchainConfiguration& config);

	/// Creates a plugin manager around \a config.
	/// \note This overload is the only overload that allows state verification.
	std::shared_ptr<plugins::PluginManager> CreatePluginManagerWithRealPlugins(const config::BitxorCoreConfiguration& config);
}}
