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
#include "tools/Options.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"

namespace bitxorcore { namespace tools {

	/// Adds 'resources' option to \a optionsBuilder.
	void AddResourcesOption(OptionsBuilder& optionsBuilder);

	/// Gets the 'resources' option value from \a options.
	std::string GetResourcesOptionValue(const Options& options);

	/// Loads the configuration from \a resourcesPath.
	config::BitxorCoreConfiguration LoadConfiguration(const std::string& resourcesPath);

	/// Loads optional api peers configuration from \a resourcesPath for network \a networkFingerprint.
	std::vector<ionet::Node> LoadOptionalApiPeers(
			const std::string& resourcesPath,
			const model::UniqueNetworkFingerprint& networkFingerprint);

	/// Loads p2p peers configuration from \a resourcesPath for network \a networkFingerprint.
	std::vector<ionet::Node> LoadPeers(const std::string& resourcesPath, const model::UniqueNetworkFingerprint& networkFingerprint);
}}
