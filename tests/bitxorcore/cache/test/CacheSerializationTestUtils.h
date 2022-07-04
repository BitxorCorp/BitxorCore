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
#include <vector>
#include <stddef.h>
#include <stdint.h>

namespace bitxorcore {
	namespace io {
		class InputStream;
		class OutputStream;
	}
}

namespace bitxorcore { namespace test {

#pragma pack(push, 1)

	/// Test entry used for cache serialization tests.
	struct CacheSerializationTestEntry {
	public:
		uint64_t Alpha;
		uint8_t Beta;
		uint32_t Gamma;

	public:
		/// Returns \c true if this entry is equal to \a rhs.
		bool operator==(const CacheSerializationTestEntry& rhs) const;
	};

#pragma pack(pop)

	/// Traits for cache serialization loader tests.
	struct CacheSerializationTestEntryLoaderTraits {
		using DestinationType = std::vector<CacheSerializationTestEntry>;

		static CacheSerializationTestEntry Load(io::InputStream& input);

		static void LoadInto(const CacheSerializationTestEntry& entry, DestinationType& destination);
	};

	/// Generates \a count random cache serialization test entries.
	std::vector<CacheSerializationTestEntry> GenerateRandomCacheSerializationTestEntries(size_t count);

	/// Copies cache serialization test \a entries to a buffer.
	std::vector<uint8_t> CopyCacheSerializationTestEntriesToStreamBuffer(const std::vector<CacheSerializationTestEntry>& entries);
}}
