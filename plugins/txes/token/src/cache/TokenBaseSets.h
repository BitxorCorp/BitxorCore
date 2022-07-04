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
#include "TokenCacheSerializers.h"
#include "TokenCacheTypes.h"
#include "bitxorcore/cache/CachePatriciaTree.h"
#include "bitxorcore/cache/PatriciaTreeEncoderAdapters.h"
#include "bitxorcore/tree/BasePatriciaTree.h"

namespace bitxorcore { namespace cache {

	using BasicTokenPatriciaTree = tree::BasePatriciaTree<
		SerializerHashedKeyEncoder<TokenCacheDescriptor::Serializer>,
		PatriciaTreeRdbDataSource,
		utils::BaseValueHasher<TokenId>>;

	class TokenPatriciaTree : public BasicTokenPatriciaTree {
	public:
		using BasicTokenPatriciaTree::BasicTokenPatriciaTree;
		using Serializer = TokenCacheDescriptor::Serializer;
	};

	struct TokenBaseSetDeltaPointers {
		TokenCacheTypes::PrimaryTypes::BaseSetDeltaPointerType pPrimary;
		TokenCacheTypes::HeightGroupingTypes::BaseSetDeltaPointerType pHeightGrouping;
		std::shared_ptr<TokenPatriciaTree::DeltaType> pPatriciaTree;
	};

	struct TokenBaseSets : public CacheDatabaseMixin {
	public:
		/// Indicates the set is not ordered.
		using IsOrderedSet = std::false_type;

	public:
		explicit TokenBaseSets(const CacheConfiguration& config)
				: CacheDatabaseMixin(config, { "default", "height_grouping" })
				, Primary(GetContainerMode(config), database(), 0)
				, HeightGrouping(GetContainerMode(config), database(), 1)
				, PatriciaTree(hasPatriciaTreeSupport(), database(), 2)
		{}

	public:
		TokenCacheTypes::PrimaryTypes::BaseSetType Primary;
		TokenCacheTypes::HeightGroupingTypes::BaseSetType HeightGrouping;
		CachePatriciaTree<TokenPatriciaTree> PatriciaTree;

	public:
		TokenBaseSetDeltaPointers rebase() {
			return { Primary.rebase(), HeightGrouping.rebase(), PatriciaTree.rebase() };
		}

		TokenBaseSetDeltaPointers rebaseDetached() const {
			return {
				Primary.rebaseDetached(),
				HeightGrouping.rebaseDetached(),
				PatriciaTree.rebaseDetached()
			};
		}

		void commit() {
			Primary.commit();
			HeightGrouping.commit();
			PatriciaTree.commit();
			flush();
		}
	};
}}
