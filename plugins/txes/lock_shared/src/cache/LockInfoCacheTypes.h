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
#include "bitxorcore/cache/CacheDatabaseMixin.h"
#include "bitxorcore/cache/CacheDescriptorAdapters.h"
#include "bitxorcore/cache/CacheSerializerAdapter.h"
#include "bitxorcore/cache/IdentifierGroupSerializer.h"
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/utils/IdentifierGroup.h"

namespace bitxorcore { namespace cache {

	/// Basic lock info cache types.
	template<typename TDescriptor>
	struct LockInfoCacheTypes {
	private:
		using IdentifierType = typename TDescriptor::KeyType;

	// region secondary descriptors

	public:
		struct HeightGroupingTypesDescriptor {
		public:
			using KeyType = Height;
			using ValueType = utils::IdentifierGroup<IdentifierType, Height, utils::ArrayHasher<IdentifierType>>;
			using Serializer = IdentifierGroupSerializer<HeightGroupingTypesDescriptor>;

		public:
			static auto GetKeyFromValue(const ValueType& heightHashes) {
				return heightHashes.key();
			}
		};

	// endregion

	public:
		using PrimaryTypes = MutableUnorderedMapAdapter<TDescriptor, utils::ArrayHasher<IdentifierType>>;
		using HeightGroupingTypes = MutableUnorderedMapAdapter<HeightGroupingTypesDescriptor, utils::BaseValueHasher<Height>>;
	};
}}

/// Defines lock info cache serializers for \a LOCK_INFO.
#define DEFINE_LOCK_INFO_CACHE_SERIALIZERS(LOCK_INFO) \
	/* Primary serializer for lock info cache. */ \
	struct LOCK_INFO##PrimarySerializer \
			: public CacheSerializerAdapter<state::LOCK_INFO##HistorySerializer, LOCK_INFO##CacheDescriptor> \
	{}; \
	\
	/* Serializer for lock info cache height grouped elements. */ \
	struct LOCK_INFO##HeightGroupingSerializer \
			: public IdentifierGroupSerializer<LOCK_INFO##CacheTypes::HeightGroupingTypesDescriptor> \
	{}; \
	\
	/* Primary serializer for lock info cache for patricia tree hashes. */ \
	/* \note This serializer excludes historical lock infos. */ \
	struct LOCK_INFO##PatriciaTreeSerializer \
			: public CacheSerializerAdapter<state::LOCK_INFO##HistoryNonHistoricalSerializer, LOCK_INFO##CacheDescriptor> \
	{};
