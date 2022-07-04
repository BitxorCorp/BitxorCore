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
#include "src/state/MetadataEntry.h"
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/cache/SingleSetCacheTypesAdapter.h"
#include "bitxorcore/utils/Hashers.h"

namespace bitxorcore {
	namespace cache {
		class BasicMetadataCacheDelta;
		class BasicMetadataCacheView;
		struct MetadataBaseSetDeltaPointers;
		struct MetadataBaseSets;
		class MetadataCache;
		class MetadataCacheDelta;
		class MetadataCacheView;
		struct MetadataEntryPrimarySerializer;
		class MetadataPatriciaTree;

		template<typename TCache, typename TCacheDelta, typename TCacheKey, typename TGetResult>
		class ReadOnlyArtifactCache;
	}
}

namespace bitxorcore { namespace cache {

	/// Describes a metadata cache.
	struct MetadataCacheDescriptor {
	public:
		static constexpr auto Name = "MetadataCache";

	public:
		// key value types
		using KeyType = Hash256;
		using ValueType = state::MetadataEntry;

		// cache types
		using CacheType = MetadataCache;
		using CacheDeltaType = MetadataCacheDelta;
		using CacheViewType = MetadataCacheView;

		using Serializer = MetadataEntryPrimarySerializer;
		using PatriciaTree = MetadataPatriciaTree;

	public:
		/// Gets the key corresponding to \a entry.
		static const auto& GetKeyFromValue(const ValueType& entry) {
			return entry.key().uniqueKey();
		}
	};

	/// Metadata cache types.
	struct MetadataCacheTypes {
		using PrimaryTypes = MutableUnorderedMapAdapter<MetadataCacheDescriptor, utils::ArrayHasher<Hash256>>;

		using CacheReadOnlyType = ReadOnlyArtifactCache<BasicMetadataCacheView, BasicMetadataCacheDelta, Hash256, state::MetadataEntry>;

		using BaseSetDeltaPointers = MetadataBaseSetDeltaPointers;
		using BaseSets = MetadataBaseSets;
	};
}}
