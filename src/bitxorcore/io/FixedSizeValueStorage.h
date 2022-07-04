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
#include "RawFile.h"
#include "bitxorcore/model/EntityRange.h"
#include "bitxorcore/model/HeightHashPair.h"

namespace bitxorcore { namespace io {

	/// Fixed size value storage.
	template<typename TKey, typename TValue>
	class FixedSizeValueStorage final {
	public:
		/// Creates storage under \a dataDirectory using \a prefix.
		FixedSizeValueStorage(const std::string& dataDirectory, const std::string& prefix);

		/// Destroys storage.
		~FixedSizeValueStorage() = default;

	public:
		/// Loads \a numValues values starting at \a key.
		model::EntityRange<TValue> loadRangeFrom(TKey key, size_t numValues) const;

		/// Saves \a value at \a key.
		/// \note Expects ascending keys.
		void save(TKey key, const TValue& value);

		/// Closes cached file.
		void reset();

	private:
		std::unique_ptr<RawFile> openStorageFile(TKey key, OpenMode openMode) const;
		void seekStorageFile(RawFile& rawFile, TKey key) const;

	private:
		std::string m_dataDirectory;
		std::string m_prefix;

		// used for caching inside save()
		uint64_t m_cachedDirectoryId;
		std::unique_ptr<RawFile> m_pCachedStorageFile;
	};

	using HashFile = FixedSizeValueStorage<Height, Hash256>;
	extern template class FixedSizeValueStorage<Height, Hash256>;
}}
