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
#include "TokenRestrictionBaseSets.h"
#include "ReadOnlyTokenRestrictionCache.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"
#include "bitxorcore/deltaset/BaseSetDelta.h"
#include "bitxorcore/model/NetworkIdentifier.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the token restriction cache delta.
	using TokenRestrictionCacheDeltaMixins =
		PatriciaTreeCacheMixins<TokenRestrictionCacheTypes::PrimaryTypes::BaseSetDeltaType, TokenRestrictionCacheDescriptor>;

	/// Basic delta on top of the token restriction cache.
	class BasicTokenRestrictionCacheDelta
			: public utils::MoveOnly
			, public TokenRestrictionCacheDeltaMixins::Size
			, public TokenRestrictionCacheDeltaMixins::Contains
			, public TokenRestrictionCacheDeltaMixins::ConstAccessor
			, public TokenRestrictionCacheDeltaMixins::MutableAccessor
			, public TokenRestrictionCacheDeltaMixins::PatriciaTreeDelta
			, public TokenRestrictionCacheDeltaMixins::BasicInsertRemove
			, public TokenRestrictionCacheDeltaMixins::DeltaElements {
	public:
		using ReadOnlyView = TokenRestrictionCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a delta around \a restrictionSets and \a networkIdentifier.
		BasicTokenRestrictionCacheDelta(
				const TokenRestrictionCacheTypes::BaseSetDeltaPointers& restrictionSets,
				model::NetworkIdentifier networkIdentifier)
				: TokenRestrictionCacheDeltaMixins::Size(*restrictionSets.pPrimary)
				, TokenRestrictionCacheDeltaMixins::Contains(*restrictionSets.pPrimary)
				, TokenRestrictionCacheDeltaMixins::ConstAccessor(*restrictionSets.pPrimary)
				, TokenRestrictionCacheDeltaMixins::MutableAccessor(*restrictionSets.pPrimary)
				, TokenRestrictionCacheDeltaMixins::PatriciaTreeDelta(*restrictionSets.pPrimary, restrictionSets.pPatriciaTree)
				, TokenRestrictionCacheDeltaMixins::BasicInsertRemove(*restrictionSets.pPrimary)
				, TokenRestrictionCacheDeltaMixins::DeltaElements(*restrictionSets.pPrimary)
				, m_pTokenRestrictionEntries(restrictionSets.pPrimary)
				, m_networkIdentifier(networkIdentifier)
		{}

	public:
		using TokenRestrictionCacheDeltaMixins::ConstAccessor::find;
		using TokenRestrictionCacheDeltaMixins::MutableAccessor::find;

	public:
		/// Gets the network identifier.
		model::NetworkIdentifier networkIdentifier() const {
			return m_networkIdentifier;
		}

	private:
		TokenRestrictionCacheTypes::PrimaryTypes::BaseSetDeltaPointerType m_pTokenRestrictionEntries;
		model::NetworkIdentifier m_networkIdentifier;
	};

	/// Delta on top of the token restriction cache.
	class TokenRestrictionCacheDelta : public ReadOnlyViewSupplier<BasicTokenRestrictionCacheDelta> {
	public:
		/// Creates a delta around \a restrictionSets and \a networkIdentifier.
		TokenRestrictionCacheDelta(
				const TokenRestrictionCacheTypes::BaseSetDeltaPointers& restrictionSets,
				model::NetworkIdentifier networkIdentifier)
				: ReadOnlyViewSupplier(restrictionSets, networkIdentifier)
		{}
	};
}}
