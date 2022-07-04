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
#include "TokenRestrictionCacheSerializers.h"
#include "ReadOnlyTokenRestrictionCache.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the token restriction cache view.
	using TokenRestrictionCacheViewMixins =
		PatriciaTreeCacheMixins<TokenRestrictionCacheTypes::PrimaryTypes::BaseSetType, TokenRestrictionCacheDescriptor>;

	/// Basic view on top of the token restriction cache.
	class BasicTokenRestrictionCacheView
			: public utils::MoveOnly
			, public TokenRestrictionCacheViewMixins::Size
			, public TokenRestrictionCacheViewMixins::Contains
			, public TokenRestrictionCacheViewMixins::Iteration
			, public TokenRestrictionCacheViewMixins::ConstAccessor
			, public TokenRestrictionCacheViewMixins::PatriciaTreeView {
	public:
		using ReadOnlyView = TokenRestrictionCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a view around \a restrictionSets and \a networkIdentifier.
		BasicTokenRestrictionCacheView(
				const TokenRestrictionCacheTypes::BaseSets& restrictionSets,
				model::NetworkIdentifier networkIdentifier)
				: TokenRestrictionCacheViewMixins::Size(restrictionSets.Primary)
				, TokenRestrictionCacheViewMixins::Contains(restrictionSets.Primary)
				, TokenRestrictionCacheViewMixins::Iteration(restrictionSets.Primary)
				, TokenRestrictionCacheViewMixins::ConstAccessor(restrictionSets.Primary)
				, TokenRestrictionCacheViewMixins::PatriciaTreeView(restrictionSets.PatriciaTree.get())
				, m_networkIdentifier(networkIdentifier)
		{}

	public:
		/// Gets the network identifier.
		model::NetworkIdentifier networkIdentifier() const {
			return m_networkIdentifier;
		}

	private:
		model::NetworkIdentifier m_networkIdentifier;
	};

	/// View on top of the token restriction cache.
	class TokenRestrictionCacheView : public ReadOnlyViewSupplier<BasicTokenRestrictionCacheView> {
	public:
		/// Creates a view around \a restrictionSets and \a networkIdentifier.
		TokenRestrictionCacheView(
				const TokenRestrictionCacheTypes::BaseSets& restrictionSets,
				model::NetworkIdentifier networkIdentifier)
				: ReadOnlyViewSupplier(restrictionSets, networkIdentifier)
		{}
	};
}}
