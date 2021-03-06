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
#include "BlockStatisticCacheMixins.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlySimpleCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the block statistic cache view.
	struct BlockStatisticCacheViewMixins
			: public BasicCacheMixins<BlockStatisticCacheTypes::PrimaryTypes::BaseSetType, BlockStatisticCacheDescriptor> {
		using BlockStatisticRange = BlockStatisticRangeMixin<BlockStatisticCacheTypes::PrimaryTypes::BaseSetType>;
	};

	/// Basic view on top of the block statistic cache.
	class BasicBlockStatisticCacheView
			: public utils::MoveOnly
			, public BlockStatisticCacheViewMixins::Size
			, public BlockStatisticCacheViewMixins::Contains
			, public BlockStatisticCacheViewMixins::Iteration
			, public BlockStatisticCacheViewMixins::BlockStatisticRange {
	public:
		using ReadOnlyView = BlockStatisticCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a view around \a statisticSets and \a options.
		BasicBlockStatisticCacheView(const BlockStatisticCacheTypes::BaseSets& statisticSets, const BlockStatisticCacheTypes::Options&)
				: BlockStatisticCacheViewMixins::Size(statisticSets.Primary)
				, BlockStatisticCacheViewMixins::Contains(statisticSets.Primary)
				, BlockStatisticCacheViewMixins::Iteration(statisticSets.Primary)
				, BlockStatisticCacheViewMixins::BlockStatisticRange(statisticSets.Primary)
		{}
	};

	/// View on top of the block statistic cache.
	class BlockStatisticCacheView : public ReadOnlyViewSupplier<BasicBlockStatisticCacheView> {
	public:
		/// Creates a view around \a statisticSets and \a options.
		BlockStatisticCacheView(const BlockStatisticCacheTypes::BaseSets& statisticSets, const BlockStatisticCacheTypes::Options& options)
				: ReadOnlyViewSupplier(statisticSets, options)
		{}
	};
}}
