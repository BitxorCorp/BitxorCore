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

#include "InflationConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace config {

	namespace {
		constexpr const char* Section_Name = "inflation";
		constexpr const char* Expected_Prefix = "starting-at-height-";

		bool GetHeightFromKey(const std::string& key, Height& height) {
			if (0 != key.find(Expected_Prefix))
				return false;

			return utils::TryParseValue(key.substr(std::strlen(Expected_Prefix)), height);
		}
	}

	InflationConfiguration InflationConfiguration::Uninitialized() {
		return InflationConfiguration();
	}

	InflationConfiguration InflationConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		if (0 == bag.size())
			BITXORCORE_THROW_AND_LOG_0(utils::property_not_found_error, "required inflation section is missing");

		InflationConfiguration config;
		auto lastHeight = Height();
		Height height;
		auto inflationMap = bag.getAllOrdered<uint64_t>(Section_Name);
		for (const auto& pair : inflationMap) {
			if (!GetHeightFromKey(pair.first, height)) {
				auto message = "property could not be parsed";
				BITXORCORE_THROW_AND_LOG_2(utils::property_malformed_error, message, std::string(Section_Name), pair.first);
			}

			config.InflationCalculator.add(height, Amount(pair.second));
			lastHeight = height;
		}

		utils::VerifyBagSizeExact(bag, config.InflationCalculator.size());
		return config;
	}
}}
