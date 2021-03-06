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
#include "src/state/Namespace.h"
#include "src/state/NamespaceEntry.h"
#include "src/state/RootNamespaceHistory.h"
#include "bitxorcore/cache/CacheDatabaseMixin.h"
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/deltaset/BaseSetDelta.h"
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/utils/IdentifierGroup.h"

namespace bitxorcore {
	namespace cache {
		class BasicNamespaceCacheDelta;
		class BasicNamespaceCacheView;
		struct NamespaceBaseSetDeltaPointers;
		struct NamespaceBaseSets;
		class NamespaceCache;
		class NamespaceCacheDelta;
		class NamespaceCacheView;
		struct NamespaceFlatMapTypesSerializer;
		struct NamespaceHeightGroupingSerializer;
		class NamespacePatriciaTree;
		class ReadOnlyNamespaceCache;
		struct RootNamespaceHistoryPrimarySerializer;
	}
}

namespace bitxorcore { namespace cache {

	/// Describes a namespace cache.
	struct NamespaceCacheDescriptor {
	public:
		static constexpr auto Name = "NamespaceCache";

	public:
		// key value types
		using KeyType = NamespaceId;
		using ValueType = state::RootNamespaceHistory;

		// cache types
		using CacheType = NamespaceCache;
		using CacheDeltaType = NamespaceCacheDelta;
		using CacheViewType = NamespaceCacheView;

		using Serializer = RootNamespaceHistoryPrimarySerializer;
		using PatriciaTree = NamespacePatriciaTree;

	public:
		/// Gets the key corresponding to \a history.
		static auto GetKeyFromValue(const ValueType& history) {
			return history.id();
		}
	};

	/// Namespace cache types.
	struct NamespaceCacheTypes {
	public:
		using CacheReadOnlyType = ReadOnlyNamespaceCache;

		/// Custom sub view options.
		struct Options {
			/// Namespace grace period duration.
			BlockDuration GracePeriodDuration;
		};

	// region secondary descriptors

	private:
		struct FlatMapTypesDescriptor {
		public:
			using KeyType = NamespaceId;
			using ValueType = state::Namespace;
			using Serializer = NamespaceFlatMapTypesSerializer;

		public:
			static auto GetKeyFromValue(const ValueType& ns) {
				return ns.id();
			}
		};

	public:
		struct HeightGroupingTypesDescriptor {
		public:
			using KeyType = Height;
			using ValueType = utils::IdentifierGroup<NamespaceId, Height, utils::BaseValueHasher<NamespaceId>>;
			using Serializer = NamespaceHeightGroupingSerializer;

		public:
			static auto GetKeyFromValue(const ValueType& heightNamespaces) {
				return heightNamespaces.key();
			}
		};

	// endregion

	public:
		using PrimaryTypes = MutableUnorderedMapAdapter<NamespaceCacheDescriptor, utils::BaseValueHasher<NamespaceId>>;
		using FlatMapTypes = MutableUnorderedMapAdapter<FlatMapTypesDescriptor, utils::BaseValueHasher<NamespaceId>>;
		using HeightGroupingTypes = MutableUnorderedMapAdapter<HeightGroupingTypesDescriptor, utils::BaseValueHasher<Height>>;

	public:
		using BaseSetDeltaPointers = NamespaceBaseSetDeltaPointers;
		using BaseSets = NamespaceBaseSets;
	};
}}
