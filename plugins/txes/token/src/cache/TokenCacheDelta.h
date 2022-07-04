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
#include "TokenBaseSets.h"
#include "TokenCacheSerializers.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"
#include "bitxorcore/deltaset/BaseSetDelta.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the token cache delta.
	struct TokenCacheDeltaMixins
			: public PatriciaTreeCacheMixins<TokenCacheTypes::PrimaryTypes::BaseSetDeltaType, TokenCacheDescriptor> {
		using Touch = HeightBasedTouchMixin<
			typename TokenCacheTypes::PrimaryTypes::BaseSetDeltaType,
			typename TokenCacheTypes::HeightGroupingTypes::BaseSetDeltaType>;
	};

	/// Basic delta on top of the token cache.
	class BasicTokenCacheDelta
			: public utils::MoveOnly
			, public TokenCacheDeltaMixins::Size
			, public TokenCacheDeltaMixins::Contains
			, public TokenCacheDeltaMixins::ConstAccessor
			, public TokenCacheDeltaMixins::MutableAccessor
			, public TokenCacheDeltaMixins::PatriciaTreeDelta
			, public TokenCacheDeltaMixins::ActivePredicate
			, public TokenCacheDeltaMixins::BasicInsertRemove
			, public TokenCacheDeltaMixins::Touch
			, public TokenCacheDeltaMixins::DeltaElements {
	public:
		using ReadOnlyView = TokenCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a delta around \a tokenSets.
		explicit BasicTokenCacheDelta(const TokenCacheTypes::BaseSetDeltaPointers& tokenSets);

	public:
		using TokenCacheDeltaMixins::ConstAccessor::find;
		using TokenCacheDeltaMixins::MutableAccessor::find;

	public:
		/// Inserts the token \a entry into the cache.
		void insert(const state::TokenEntry& entry);

		/// Removes the value identified by \a tokenId from the cache.
		void remove(TokenId tokenId);

	private:
		TokenCacheTypes::PrimaryTypes::BaseSetDeltaPointerType m_pEntryById;
		TokenCacheTypes::HeightGroupingTypes::BaseSetDeltaPointerType m_pTokenIdsByExpiryHeight;
	};

	/// Delta on top of the token cache.
	class TokenCacheDelta : public ReadOnlyViewSupplier<BasicTokenCacheDelta> {
	public:
		/// Creates a delta around \a tokenSets.
		explicit TokenCacheDelta(const TokenCacheTypes::BaseSetDeltaPointers& tokenSets) : ReadOnlyViewSupplier(tokenSets)
		{}
	};
}}
