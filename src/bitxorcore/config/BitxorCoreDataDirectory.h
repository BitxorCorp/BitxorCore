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
#include "bitxorcore/constants.h"
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace bitxorcore { namespace config {

	// region BitxorCoreDirectory

	/// BitxorCore directory.
	class BitxorCoreDirectory {
	public:
		/// Creates a directory around \a directory.
		explicit BitxorCoreDirectory(const std::filesystem::path& directory) : m_directory(directory)
		{}

	public:
		/// Gets the directory path (as string).
		std::string str() const {
			return m_directory.generic_string();
		}

		/// Gets the directory path (as filesystem::path).
		const std::filesystem::path& path() const {
			return m_directory;
		}

		/// Gets the path for the file with \a name.
		std::string file(const std::string& name) const {
			return (m_directory / name).generic_string();
		}

		/// Gets the directory with \a name.
		BitxorCoreDirectory dir(const std::string& name) const {
			return BitxorCoreDirectory(m_directory / name);
		}

	public:
		/// Returns \c true if directory exits.
		bool exists() const;

		/// Creates this directory in a filesystem.
		void create() const;

		/// Creates all directories on the path to this directory in a filesystem.
		void createAll() const;

	private:
		std::filesystem::path m_directory;
	};

	// endregion

	// region BitxorCoreStorageDirectory

	/// BitxorCore storage directory.
	class BitxorCoreStorageDirectory {
	public:
		/// Creates a storage directory around \a directory and \a identifier
		template<typename TIdentifier>
		BitxorCoreStorageDirectory(const std::filesystem::path& directory, TIdentifier identifier)
				: m_directory(directory)
				, m_identifier(identifier.unwrap())
		{}

	public:
		/// Gets the directory path (as string).
		std::string str() const {
			return m_directory.str();
		}

		/// Gets the path for a storage file with \a extension.
		std::string storageFile(const std::string& extension) const {
			std::ostringstream buffer;
			buffer << std::setfill('0') << std::setw(5) << (m_identifier % Files_Per_Storage_Directory);
			buffer << extension;

			return m_directory.file(buffer.str());
		}

		/// Gets the path for an index file built around \a prefix and \a extension.
		std::string indexFile(const std::string& prefix, const std::string& extension) const {
			return m_directory.file(prefix + extension);
		}

	private:
		BitxorCoreDirectory m_directory;
		uint64_t m_identifier;
	};

	// endregion

	// region BitxorCoreDataDirectory

	/// BitxorCore data directory.
	class BitxorCoreDataDirectory {
	public:
		/// Creates a data directory around \a directory.
		explicit BitxorCoreDataDirectory(const std::filesystem::path& directory) : m_directory(directory)
		{}

	public:
		/// Gets the root data directory.
		BitxorCoreDirectory rootDir() const {
			return BitxorCoreDirectory(m_directory);
		}

		/// Gets the directory with \a name.
		BitxorCoreDirectory dir(const std::string& name) const {
			return BitxorCoreDirectory(m_directory / name);
		}

		/// Gets the spooling directory with \a name.
		BitxorCoreDirectory spoolDir(const std::string& name) const {
			return BitxorCoreDirectory(m_directory / "spool" / name);
		}

		/// Gets the storage directory built around \a identifier.
		template<typename TIdentifier>
		BitxorCoreStorageDirectory storageDir(TIdentifier identifier) {
			std::ostringstream buffer;
			buffer << std::setfill('0') << std::setw(5) << (identifier.unwrap() / Files_Per_Storage_Directory);
			auto subDirectory = m_directory / buffer.str();
			return BitxorCoreStorageDirectory(subDirectory, identifier);
		}

	private:
		std::filesystem::path m_directory;
	};

	// endregion

	// region BitxorCoreDataDirectoryPreparer

	/// BitxorCore data directory factory that automatically creates subdirectories.
	class BitxorCoreDataDirectoryPreparer {
	public:
		/// Creates a data directory around \a directory.
		static BitxorCoreDataDirectory Prepare(const std::filesystem::path& directory) {
			BitxorCoreDirectory(directory / "importance" / "wip").createAll();
			BitxorCoreDirectory(directory / "spool").create();
			return BitxorCoreDataDirectory(directory);
		}
	};

	// endregion

	// region BitxorCoreStorageDirectoryPreparer

	/// BitxorCore storage directory factory that creates subdirectories.
	class BitxorCoreStorageDirectoryPreparer {
	public:
		/// Creates a storage directory around \a directory and \a identifier.
		template<typename TIdentifier>
		static BitxorCoreStorageDirectory Prepare(const std::filesystem::path& directory, TIdentifier identifier) {
			BitxorCoreDataDirectory dataDirectory(directory);
			auto storageDirectory = dataDirectory.storageDir(identifier);
			BitxorCoreDirectory(storageDirectory.str()).create();
			return storageDirectory;
		}
	};

	// endregion
}}
