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

#include "NodeVersion.h"
#include "bitxorcore/utils/ConfigurationValueParsers.h"
#include "bitxorcore/version/version.h"

namespace bitxorcore { namespace ionet {

	namespace {
		static constexpr auto Num_Version_Parts = 4u;

		NodeVersion PackVersion(const std::array<uint8_t, Num_Version_Parts>& versionParts) {
			NodeVersion::ValueType version = 0;
			for (auto part : versionParts) {
				version <<= 8;
				version |= part;
			}

			return NodeVersion(version);
		}
	}

	NodeVersion GetCurrentServerVersion() {
		return PackVersion({ BITXORCORE_VERSION_MAJOR, BITXORCORE_VERSION_MINOR, BITXORCORE_VERSION_REVISION, BITXORCORE_VERSION_BUILD });
	}

	bool TryParseValue(const std::string& str, NodeVersion& version) {
		if (str.empty()) {
			version = GetCurrentServerVersion();
			return true;
		}

		size_t searchIndex = 0;
		std::vector<std::string> versionParts;
		while (true) {
			auto separatorIndex = str.find('.', searchIndex);
			auto item = std::string::npos == separatorIndex
					? str.substr(searchIndex)
					: str.substr(searchIndex, separatorIndex - searchIndex);

			// don't allow empty values
			if (item.empty())
				return false;

			versionParts.push_back(item);
			if (std::string::npos == separatorIndex)
				break;

			searchIndex = separatorIndex + 1;
		}

		if (Num_Version_Parts != versionParts.size())
			return false;

		std::array<uint8_t, Num_Version_Parts> parsedVersionParts;
		for (auto i = 0u; i < Num_Version_Parts; ++i) {
			if (!utils::TryParseValue(versionParts[i], parsedVersionParts[i]))
				return false;
		}

		version = PackVersion(parsedVersionParts);
		return true;
	}
}}
