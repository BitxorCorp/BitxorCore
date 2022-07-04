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
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/cache/SingleSetCacheTypesAdapter.h"
#include "bitxorcore/state/TimestampedHash.h"
#include "bitxorcore/utils/TimeSpan.h"

namespace bitxorcore {
	namespace cache {
		class BasicHashCacheDelta;
		class BasicHashCacheView;
		class HashCache;
		class HashCacheDelta;
		struct HashCachePrimarySerializer;
		class HashCacheView;

		template<typename TCache, typename TCacheDelta, typename TCacheKey>
		class ReadOnlySimpleCache;
	}
}

namespace bitxorcore { namespace cache {

	/// Describes a hash cache.
	struct HashCacheDescriptor {
	public:
		static constexpr auto Name = "HashCache";

	public:
		// key value types
		using KeyType = state::TimestampedHash;
		using ValueType = state::TimestampedHash;
		using Serializer = HashCachePrimarySerializer;

		// cache types
		using CacheType = HashCache;
		using CacheDeltaType = HashCacheDelta;
		using CacheViewType = HashCacheView;

	public:
		/// Gets the key corresponding to \a timestampedHash.
		static const auto& GetKeyFromValue(const ValueType& timestampedHash) {
			return timestampedHash;
		}
	};

	/// Hash cache types.
	struct HashCacheTypes : public SingleSetCacheTypesAdapter<ImmutableOrderedSetAdapter<HashCacheDescriptor>, std::true_type> {
		using CacheReadOnlyType = ReadOnlySimpleCache<BasicHashCacheView, BasicHashCacheDelta, state::TimestampedHash>;

		/// Custom sub view options.
		struct Options {
			/// Cache retention time.
			utils::TimeSpan RetentionTime;
		};
	};
}}
