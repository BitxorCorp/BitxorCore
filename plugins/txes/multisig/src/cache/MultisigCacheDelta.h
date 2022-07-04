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
#include "MultisigBaseSets.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlyArtifactCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"
#include "bitxorcore/deltaset/BaseSetDelta.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the multisig cache delta.
	using MultisigCacheDeltaMixins = PatriciaTreeCacheMixins<MultisigCacheTypes::PrimaryTypes::BaseSetDeltaType, MultisigCacheDescriptor>;

	/// Basic delta on top of the multisig cache.
	class BasicMultisigCacheDelta
			: public utils::MoveOnly
			, public MultisigCacheDeltaMixins::Size
			, public MultisigCacheDeltaMixins::Contains
			, public MultisigCacheDeltaMixins::ConstAccessor
			, public MultisigCacheDeltaMixins::MutableAccessor
			, public MultisigCacheDeltaMixins::PatriciaTreeDelta
			, public MultisigCacheDeltaMixins::BasicInsertRemove
			, public MultisigCacheDeltaMixins::DeltaElements {
	public:
		using ReadOnlyView = MultisigCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a delta around \a multisigSets.
		explicit BasicMultisigCacheDelta(const MultisigCacheTypes::BaseSetDeltaPointers& multisigSets)
				: MultisigCacheDeltaMixins::Size(*multisigSets.pPrimary)
				, MultisigCacheDeltaMixins::Contains(*multisigSets.pPrimary)
				, MultisigCacheDeltaMixins::ConstAccessor(*multisigSets.pPrimary)
				, MultisigCacheDeltaMixins::MutableAccessor(*multisigSets.pPrimary)
				, MultisigCacheDeltaMixins::PatriciaTreeDelta(*multisigSets.pPrimary, multisigSets.pPatriciaTree)
				, MultisigCacheDeltaMixins::BasicInsertRemove(*multisigSets.pPrimary)
				, MultisigCacheDeltaMixins::DeltaElements(*multisigSets.pPrimary)
				, m_pMultisigEntries(multisigSets.pPrimary)
		{}

	public:
		using MultisigCacheDeltaMixins::ConstAccessor::find;
		using MultisigCacheDeltaMixins::MutableAccessor::find;

	private:
		MultisigCacheTypes::PrimaryTypes::BaseSetDeltaPointerType m_pMultisigEntries;
	};

	/// Delta on top of the multisig cache.
	class MultisigCacheDelta : public ReadOnlyViewSupplier<BasicMultisigCacheDelta> {
	public:
		/// Creates a delta around \a multisigSets.
		explicit MultisigCacheDelta(const MultisigCacheTypes::BaseSetDeltaPointers& multisigSets)
				: ReadOnlyViewSupplier(multisigSets)
		{}
	};
}}
