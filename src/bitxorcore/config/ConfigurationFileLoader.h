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
#include "PeersConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include <filesystem>
#include <iostream>

namespace bitxorcore { namespace config {

	/// Loads configuration from \a path using \a loader.
	template<
			typename TConfigurationLoader,
			typename TConfiguration = std::invoke_result_t<TConfigurationLoader, const std::string&>
	>
	TConfiguration LoadConfiguration(const std::filesystem::path& path, TConfigurationLoader loader) {
		if (!std::filesystem::exists(path)) {
			auto message = "aborting load due to missing configuration file";
			BITXORCORE_LOG(fatal) << message << ": " << path;
			BITXORCORE_THROW_EXCEPTION(bitxorcore_runtime_error(message));
		}

		std::cout << "loading configuration from " << path << std::endl;
		return loader(path.generic_string());
	}

	/// Loads ini configuration from \a path.
	template<typename TConfiguration>
	TConfiguration LoadIniConfiguration(const std::filesystem::path& path) {
		return LoadConfiguration(path, [](const auto& filePath) {
			return TConfiguration::LoadFromBag(utils::ConfigurationBag::FromPath(filePath));
		});
	}

	/// Loads peers configuration from \a path for network \a networkFingerprint.
	inline std::vector<ionet::Node> LoadPeersConfiguration(
			const std::filesystem::path& path,
			const model::UniqueNetworkFingerprint& networkFingerprint) {
		return LoadConfiguration(path, [networkFingerprint](const auto& filePath) {
			return LoadPeersFromPath(filePath, networkFingerprint);
		});
	}
}}
