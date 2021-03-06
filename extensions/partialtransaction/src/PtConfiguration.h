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
#include "bitxorcore/utils/FileSize.h"
#include <filesystem>

namespace bitxorcore { namespace utils { class ConfigurationBag; } }

namespace bitxorcore { namespace partialtransaction {

	/// Partial transactions configuration settings.
	struct PtConfiguration {
	public:
		/// Maximum size of a partial transactions response.
		utils::FileSize CacheMaxResponseSize;

		/// Maximum size of the partial transactions cache.
		utils::FileSize CacheMaxSize;

	private:
		PtConfiguration() = default;

	public:
		/// Creates an uninitialized partial transactions configuration.
		static PtConfiguration Uninitialized();

	public:
		/// Loads a partial transactions configuration from \a bag.
		static PtConfiguration LoadFromBag(const utils::ConfigurationBag& bag);

		/// Loads a partial transactions configuration from \a resourcesPath.
		static PtConfiguration LoadFromPath(const std::filesystem::path& resourcesPath);
	};
}}
