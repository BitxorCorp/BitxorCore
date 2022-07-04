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
#include "ExtensionsConfiguration.h"
#include "InflationConfiguration.h"
#include "LoggingConfiguration.h"
#include "NodeConfiguration.h"
#include "PeersConfiguration.h"
#include "UserConfiguration.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include <filesystem>

namespace bitxorcore { namespace config {

	/// Comprehensive configuration for a bitxorcore process.
	class BitxorCoreConfiguration {
	public:
		/// Creates a bitxorcore configuration around \a blockchainConfig, \a nodeConfig, \a loggingConfig, \a userConfig,
		/// \a extensionsConfig and \a inflationConfig.
		BitxorCoreConfiguration(
				model::BlockchainConfiguration&& blockchainConfig,
				NodeConfiguration&& nodeConfig,
				LoggingConfiguration&& loggingConfig,
				UserConfiguration&& userConfig,
				ExtensionsConfiguration&& extensionsConfig,
				InflationConfiguration&& inflationConfig);

	public:
		/// Blockchain configuration.
		const model::BlockchainConfiguration Blockchain;

		/// Node configuration.
		const NodeConfiguration Node;

		/// Logging configuration.
		const LoggingConfiguration Logging;

		/// User configuration.
		const UserConfiguration User;

		/// Extensions configuration.
		const ExtensionsConfiguration Extensions;

		/// Inflation configuration.
		const InflationConfiguration Inflation;

	public:
		/// Loads a bitxorcore configuration from \a resourcesPath given the specified extensions host (\a extensionsHost).
		/// \note This function is expected to be called be before logging is enabled.
		static BitxorCoreConfiguration LoadFromPath(const std::filesystem::path& resourcesPath, const std::string& extensionsHost);
	};

	/// Extracts a node representing the local node from \a config.
	ionet::Node ToLocalNode(const BitxorCoreConfiguration& config);
}}
