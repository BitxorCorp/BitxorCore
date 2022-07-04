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
#include "src/state/TokenEntry.h"
#include "bitxorcore/cache/CacheDatabaseMixin.h"
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/utils/IdentifierGroup.h"

namespace bitxorcore {
	namespace cache {
		class BasicTokenCacheDelta;
		class BasicTokenCacheView;
		struct TokenBaseSetDeltaPointers;
		struct TokenBaseSets;
		class TokenCache;
		class TokenCacheDelta;
		class TokenCacheView;
		struct TokenEntryPrimarySerializer;
		struct TokenHeightGroupingSerializer;
		class TokenPatriciaTree;

		template<typename TCache, typename TCacheDelta, typename TCacheKey, typename TGetResult>
		class ReadOnlyArtifactCache;
	}
}

namespace bitxorcore { namespace cache {

	/// Describes a token cache.
	struct TokenCacheDescriptor {
	public:
		static constexpr auto Name = "TokenCache";

	public:
		// key value types
		using KeyType = TokenId;
		using ValueType = state::TokenEntry;

		// cache types
		using CacheType = TokenCache;
		using CacheDeltaType = TokenCacheDelta;
		using CacheViewType = TokenCacheView;

		using Serializer = TokenEntryPrimarySerializer;
		using PatriciaTree = TokenPatriciaTree;

	public:
		/// Gets the key corresponding to \a entry.
		static auto GetKeyFromValue(const ValueType& entry) {
			return entry.tokenId();
		}
	};

	/// Token cache types.
	struct TokenCacheTypes {
	public:
		using CacheReadOnlyType = ReadOnlyArtifactCache<BasicTokenCacheView, BasicTokenCacheDelta, TokenId, state::TokenEntry>;

	// region secondary descriptors

	public:
		struct HeightGroupingTypesDescriptor {
		public:
			using KeyType = Height;
			using ValueType = utils::IdentifierGroup<TokenId, Height, utils::BaseValueHasher<TokenId>>;
			using Serializer = TokenHeightGroupingSerializer;

		public:
			static auto GetKeyFromValue(const ValueType& heightTokens) {
				return heightTokens.key();
			}
		};

	// endregion

	public:
		using PrimaryTypes = MutableUnorderedMapAdapter<TokenCacheDescriptor, utils::BaseValueHasher<TokenId>>;
		using HeightGroupingTypes = MutableUnorderedMapAdapter<HeightGroupingTypesDescriptor, utils::BaseValueHasher<Height>>;

	public:
		using BaseSetDeltaPointers = TokenBaseSetDeltaPointers;
		using BaseSets = TokenBaseSets;
	};
}}
