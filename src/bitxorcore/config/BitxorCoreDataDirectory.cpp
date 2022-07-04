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

#include "BitxorCoreDataDirectory.h"
#include "bitxorcore/exceptions.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace bitxorcore { namespace config {

	namespace {
#ifdef _WIN32
		bool CreateDirectory(const std::filesystem::path& directory) {
			std::error_code ec;
			std::filesystem::create_directory(directory, ec);
			return !ec;
		}
#else
		bool CreateDirectory(const std::filesystem::path& directory) {
			return !mkdir(directory.generic_string().c_str(), 0700);
		}
#endif
	}

	bool BitxorCoreDirectory::exists() const {
		return std::filesystem::is_directory(m_directory);
	}

	void BitxorCoreDirectory::create() const {
		if (exists())
			return;

		if (!CreateDirectory(m_directory))
			BITXORCORE_THROW_RUNTIME_ERROR_1("couldn't create directory", m_directory.generic_string());
	}

	void BitxorCoreDirectory::createAll() const {
		std::filesystem::path currentDirectory;

		for (const auto& part : m_directory) {
			if (currentDirectory.empty())
				currentDirectory = part;
			else
				currentDirectory /= part;

			BitxorCoreDirectory(currentDirectory).create();
		}
	}
}}
