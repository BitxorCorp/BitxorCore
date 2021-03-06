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
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"
#include "bitxorcore/deltaset/BaseSetDelta.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the metadata cache delta.
	using MetadataCacheDeltaMixins = PatriciaTreeCacheMixins<MetadataCacheTypes::PrimaryTypes::BaseSetDeltaType, MetadataCacheDescriptor>;

	/// Basic delta on top of the metadata cache.
	class BasicMetadataCacheDelta
			: public utils::MoveOnly
			, public MetadataCacheDeltaMixins::Size
			, public MetadataCacheDeltaMixins::Contains
			, public MetadataCacheDeltaMixins::ConstAccessor
			, public MetadataCacheDeltaMixins::MutableAccessor
			, public MetadataCacheDeltaMixins::PatriciaTreeDelta
			, public MetadataCacheDeltaMixins::BasicInsertRemove
			, public MetadataCacheDeltaMixins::DeltaElements {
	public:
		using ReadOnlyView = MetadataCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a delta around \a metadataSets.
		explicit BasicMetadataCacheDelta(const MetadataCacheTypes::BaseSetDeltaPointers& metadataSets)
				: MetadataCacheDeltaMixins::Size(*metadataSets.pPrimary)
				, MetadataCacheDeltaMixins::Contains(*metadataSets.pPrimary)
				, MetadataCacheDeltaMixins::ConstAccessor(*metadataSets.pPrimary)
				, MetadataCacheDeltaMixins::MutableAccessor(*metadataSets.pPrimary)
				, MetadataCacheDeltaMixins::PatriciaTreeDelta(*metadataSets.pPrimary, metadataSets.pPatriciaTree)
				, MetadataCacheDeltaMixins::BasicInsertRemove(*metadataSets.pPrimary)
				, MetadataCacheDeltaMixins::DeltaElements(*metadataSets.pPrimary)
				, m_pMetadataEntries(metadataSets.pPrimary)
		{}

	public:
		using MetadataCacheDeltaMixins::ConstAccessor::find;
		using MetadataCacheDeltaMixins::MutableAccessor::find;

	private:
		MetadataCacheTypes::PrimaryTypes::BaseSetDeltaPointerType m_pMetadataEntries;
	};

	/// Delta on top of the metadata cache.
	class MetadataCacheDelta : public ReadOnlyViewSupplier<BasicMetadataCacheDelta> {
	public:
		/// Creates a delta around \a metadataSets.
		explicit MetadataCacheDelta(const MetadataCacheTypes::BaseSetDeltaPointers& metadataSets) : ReadOnlyViewSupplier(metadataSets)
		{}
	};
}}
