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
#include <string>

namespace bitxorcore { namespace test {

	/// Adds configuration for all plugins required by the default genesis block to \a config.
	void AddGenesisPluginExtensions(model::BlockchainConfiguration& config);

	/// Adds configuration for all extensions required by api nodes to \a config.
	void AddApiPluginExtensions(config::ExtensionsConfiguration& config);

	/// Adds configuration for all extensions required by p2p nodes to \a config.
	void AddPeerPluginExtensions(config::ExtensionsConfiguration& config);

	/// Adds configuration for all extensions required by simple partner nodes to \a config.
	void AddSimplePartnerPluginExtensions(config::ExtensionsConfiguration& config);

	/// Adds configuration for all extensions required by recovery process to \a config.
	void AddRecoveryPluginExtensions(config::ExtensionsConfiguration& config);

	/// Creates a test bitxorcore configuration with a storage in the specified directory (\a dataDirectory)
	/// that includes configuration for all plugins required by the default genesis block.
	config::BitxorCoreConfiguration CreateBitxorCoreConfigurationWithGenesisPluginExtensions(const std::string& dataDirectory);
}}
