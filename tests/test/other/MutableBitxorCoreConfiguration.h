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

namespace bitxorcore { namespace test {

	/// Comprehensive mutable configuration for a bitxorcore process.
	class MutableBitxorCoreConfiguration {
	public:
		/// Creates a mutable bitxorcore configuration.
		MutableBitxorCoreConfiguration()
				: Blockchain(model::BlockchainConfiguration::Uninitialized())
				, Node(config::NodeConfiguration::Uninitialized())
				, Logging(config::LoggingConfiguration::Uninitialized())
				, User(config::UserConfiguration::Uninitialized())
				, Extensions(config::ExtensionsConfiguration::Uninitialized())
				, Inflation(config::InflationConfiguration::Uninitialized())
		{}

	public:
		/// Blockchain configuration.
		model::BlockchainConfiguration Blockchain;

		/// Node configuration.
		config::NodeConfiguration Node;

		/// Logging configuration.
		config::LoggingConfiguration Logging;

		/// User configuration.
		config::UserConfiguration User;

		/// Extensions configuration.
		config::ExtensionsConfiguration Extensions;

		/// Inflation configuration.
		config::InflationConfiguration Inflation;

	public:
		/// Converts this mutable configuration to a const configuration.
		config::BitxorCoreConfiguration ToConst() {
			return config::BitxorCoreConfiguration(
					std::move(Blockchain),
					std::move(Node),
					std::move(Logging),
					std::move(User),
					std::move(Extensions),
					std::move(Inflation));
		}
	};
}}
