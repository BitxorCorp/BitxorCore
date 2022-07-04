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
#include "MetadataBaseSets.h"
#include "MetadataCacheSerializers.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the metadata cache view.
	using MetadataCacheViewMixins = PatriciaTreeCacheMixins<MetadataCacheTypes::PrimaryTypes::BaseSetType, MetadataCacheDescriptor>;

	/// Basic view on top of the metadata cache.
	class BasicMetadataCacheView
			: public utils::MoveOnly
			, public MetadataCacheViewMixins::Size
			, public MetadataCacheViewMixins::Contains
			, public MetadataCacheViewMixins::Iteration
			, public MetadataCacheViewMixins::ConstAccessor
			, public MetadataCacheViewMixins::PatriciaTreeView {
	public:
		using ReadOnlyView = MetadataCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a view around \a metadataSets.
		explicit BasicMetadataCacheView(const MetadataCacheTypes::BaseSets& metadataSets)
				: MetadataCacheViewMixins::Size(metadataSets.Primary)
				, MetadataCacheViewMixins::Contains(metadataSets.Primary)
				, MetadataCacheViewMixins::Iteration(metadataSets.Primary)
				, MetadataCacheViewMixins::ConstAccessor(metadataSets.Primary)
				, MetadataCacheViewMixins::PatriciaTreeView(metadataSets.PatriciaTree.get())
		{}
	};

	/// View on top of the metadata cache.
	class MetadataCacheView : public ReadOnlyViewSupplier<BasicMetadataCacheView> {
	public:
		/// Creates a view around \a metadataSets.
		explicit MetadataCacheView(const MetadataCacheTypes::BaseSets& metadataSets) : ReadOnlyViewSupplier(metadataSets)
		{}
	};
}}
