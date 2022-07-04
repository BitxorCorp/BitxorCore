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
#include "src/state/TokenRestrictionEntry.h"
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/cache/SingleSetCacheTypesAdapter.h"
#include "bitxorcore/utils/Hashers.h"

namespace bitxorcore {
	namespace cache {
		class BasicTokenRestrictionCacheDelta;
		class BasicTokenRestrictionCacheView;
		struct TokenRestrictionBaseSetDeltaPointers;
		struct TokenRestrictionBaseSets;
		class TokenRestrictionCache;
		class TokenRestrictionCacheDelta;
		class TokenRestrictionCacheView;
		struct TokenRestrictionEntryPrimarySerializer;
		class TokenRestrictionPatriciaTree;
		class ReadOnlyTokenRestrictionCache;
	}
}

namespace bitxorcore { namespace cache {

	/// Describes a token restriction cache.
	struct TokenRestrictionCacheDescriptor {
	public:
		static constexpr auto Name = "TokenRestrictionCache";

	public:
		// key value types
		using KeyType = Hash256;
		using ValueType = state::TokenRestrictionEntry;

		// cache types
		using CacheType = TokenRestrictionCache;
		using CacheDeltaType = TokenRestrictionCacheDelta;
		using CacheViewType = TokenRestrictionCacheView;

		using Serializer = TokenRestrictionEntryPrimarySerializer;
		using PatriciaTree = TokenRestrictionPatriciaTree;

	public:
		/// Gets the key corresponding to \a entry.
		static auto GetKeyFromValue(const ValueType& entry) {
			return entry.uniqueKey();
		}
	};

	/// Token restriction cache types.
	struct TokenRestrictionCacheTypes {
		using PrimaryTypes = MutableUnorderedMapAdapter<TokenRestrictionCacheDescriptor, utils::ArrayHasher<Hash256>>;

		using CacheReadOnlyType = ReadOnlyTokenRestrictionCache;

		using BaseSetDeltaPointers = TokenRestrictionBaseSetDeltaPointers;
		using BaseSets = TokenRestrictionBaseSets;
	};
}}
