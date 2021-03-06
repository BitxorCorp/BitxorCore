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
#include "HashCacheSerializers.h"
#include "HashCacheTypes.h"
#include "bitxorcore/cache/CacheMixinAliases.h"
#include "bitxorcore/cache/ReadOnlySimpleCache.h"
#include "bitxorcore/cache/ReadOnlyViewSupplier.h"

namespace bitxorcore { namespace cache {

	/// Mixins used by the hash cache view.
	using HashCacheViewMixins = BasicCacheMixins<HashCacheTypes::PrimaryTypes::BaseSetType, HashCacheDescriptor>;

	/// Basic view on top of the hash cache.
	class BasicHashCacheView
			: public utils::MoveOnly
			, public HashCacheViewMixins::Size
			, public HashCacheViewMixins::Contains
			, public HashCacheViewMixins::Iteration {
	public:
		using ReadOnlyView = HashCacheTypes::CacheReadOnlyType;

	public:
		/// Creates a view around \a hashSets and \a options.
		BasicHashCacheView(const HashCacheTypes::BaseSets& hashSets, const HashCacheTypes::Options& options)
				: HashCacheViewMixins::Size(hashSets.Primary)
				, HashCacheViewMixins::Contains(hashSets.Primary)
				, HashCacheViewMixins::Iteration(hashSets.Primary)
				, m_retentionTime(options.RetentionTime)
		{}

	public:
		/// Gets the retention time for the cache.
		utils::TimeSpan retentionTime() const {
			return m_retentionTime;
		}

	private:
		utils::TimeSpan m_retentionTime;
	};

	/// View on top of the hash cache.
	class HashCacheView : public ReadOnlyViewSupplier<BasicHashCacheView> {
	public:
		/// Creates a view around \a hashSets and \a options.
		HashCacheView(const HashCacheTypes::BaseSets& hashSets, const HashCacheTypes::Options& options)
				: ReadOnlyViewSupplier(hashSets, options)
		{}
	};
}}
