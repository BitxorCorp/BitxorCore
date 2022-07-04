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

#include "DatabaseConfiguration.h"
#include "bitxorcore/config/ConfigurationFileLoader.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace mongo {

#define LOAD_PROPERTY(SECTION, NAME) utils::LoadIniProperty(bag, SECTION, #NAME, config.NAME)

	DatabaseConfiguration DatabaseConfiguration::Uninitialized() {
		return DatabaseConfiguration();
	}

	DatabaseConfiguration DatabaseConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		DatabaseConfiguration config;

#define LOAD_DB_PROPERTY(NAME) LOAD_PROPERTY("database", NAME)

		LOAD_DB_PROPERTY(DatabaseUri);
		LOAD_DB_PROPERTY(DatabaseName);
		LOAD_DB_PROPERTY(MaxWriterThreads);
		LOAD_DB_PROPERTY(MaxDropBatchSize);
		LOAD_DB_PROPERTY(WriteTimeout);

#undef LOAD_DB_PROPERTY

		auto pluginsPair = utils::ExtractSectionAsUnorderedSet(bag, "plugins");
		config.Plugins = pluginsPair.first;

		utils::VerifyBagSizeExact(bag, 5 + pluginsPair.second);
		return config;
	}

#undef LOAD_PROPERTY

	DatabaseConfiguration DatabaseConfiguration::LoadFromPath(const std::filesystem::path& resourcesPath) {
		return config::LoadIniConfiguration<DatabaseConfiguration>(resourcesPath / "config-database.properties");
	}
}}
