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
#include "bitxorcore/plugins/PluginManager.h"
#include "tests/test/nodeps/Conversions.h"

namespace bitxorcore { namespace test {

	/// Creates a plugin manager around \a config and \a userConfig.
	inline plugins::PluginManager CreatePluginManager(
			const model::BlockchainConfiguration& config,
			const config::UserConfiguration& userConfig) {
		return plugins::PluginManager(
				config,
				plugins::StorageConfiguration(),
				userConfig,
				config::InflationConfiguration::Uninitialized());
	}

	/// Creates a plugin manager around \a config.
	inline plugins::PluginManager CreatePluginManager(const model::BlockchainConfiguration& config) {
		auto userConfig = config::UserConfiguration::Uninitialized();
		return CreatePluginManager(config, userConfig);
	}

	/// Creates a default plugin manager.
	inline plugins::PluginManager CreatePluginManager() {
		return CreatePluginManager(model::BlockchainConfiguration::Uninitialized());
	}
}}
