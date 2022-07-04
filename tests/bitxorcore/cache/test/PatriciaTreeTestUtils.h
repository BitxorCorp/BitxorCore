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
#include "bitxorcore/cache/PatriciaTreeEncoderAdapters.h"
#include "bitxorcore/tree/BasePatriciaTree.h"
#include "bitxorcore/tree/MemoryDataSource.h"
#include <vector>

namespace bitxorcore { namespace test {

	/// Serializer for uint32_t key and std::string value.
	struct MemoryPatriciaTreeSimpleSerializer {
		using KeyType = uint32_t;
		using ValueType = std::string;

		static const std::string& SerializeValue(const ValueType& value) {
			return value;
		}
	};

	/// Memory patricia tree used in cache tests.
	using MemoryPatriciaTree = tree::PatriciaTree<
		cache::SerializerPlainKeyEncoder<MemoryPatriciaTreeSimpleSerializer>,
		tree::MemoryDataSource>;

	/// Memory base patricia tree used in cache tests.
	using MemoryBasePatriciaTree = tree::BasePatriciaTree<
		cache::SerializerPlainKeyEncoder<MemoryPatriciaTreeSimpleSerializer>,
		tree::MemoryDataSource>;

	/// Seeds \a tree with four nodes.
	void SeedTreeWithFourNodes(MemoryPatriciaTree& tree);

	/// Seeds \a tree with four nodes.
	void SeedTreeWithFourNodes(MemoryBasePatriciaTree& tree);

	/// Calculates the expected patricia tree root hash given nodes represented by \a pairs.
	Hash256 CalculateRootHash(const std::vector<std::pair<uint32_t, std::string>>& pairs);
}}
