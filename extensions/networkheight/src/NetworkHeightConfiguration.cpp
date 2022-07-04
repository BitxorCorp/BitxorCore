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

#include "NetworkHeightConfiguration.h"
#include "bitxorcore/config/ConfigurationFileLoader.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace networkheight {

#define LOAD_PROPERTY(SECTION, NAME) utils::LoadIniProperty(bag, SECTION, #NAME, config.NAME)

	NetworkHeightConfiguration NetworkHeightConfiguration::Uninitialized() {
		return NetworkHeightConfiguration();
	}

	NetworkHeightConfiguration NetworkHeightConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		NetworkHeightConfiguration config;

#define LOAD_NETWORKHEIGHT_PROPERTY(NAME) LOAD_PROPERTY("networkheight", NAME)

		LOAD_NETWORKHEIGHT_PROPERTY(MaxNodes);

#undef LOAD_NETWORKHEIGHT_PROPERTY

		utils::VerifyBagSizeExact(bag, 1);
		return config;
	}

#undef LOAD_PROPERTY

	NetworkHeightConfiguration NetworkHeightConfiguration::LoadFromPath(const std::filesystem::path& resourcesPath) {
		return config::LoadIniConfiguration<NetworkHeightConfiguration>(resourcesPath / "config-networkheight.properties");
	}
}}
