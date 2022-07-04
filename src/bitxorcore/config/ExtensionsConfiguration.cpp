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

#include "ExtensionsConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace config {

	ExtensionsConfiguration ExtensionsConfiguration::Uninitialized() {
		return ExtensionsConfiguration();
	}

	ExtensionsConfiguration ExtensionsConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		ExtensionsConfiguration config;

		if (0 == bag.size())
			BITXORCORE_THROW_AND_LOG_0(utils::property_not_found_error, "required extensions section is missing");

		auto extensionsPair = utils::ExtractSectionAsOrderedVector(bag, "extensions");
		config.Names = extensionsPair.first;

		utils::VerifyBagSizeExact(bag, extensionsPair.second);
		return config;
	}
}}
