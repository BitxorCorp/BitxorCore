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
#include "BaseValue.h"
#include "bitxorcore/types.h"
#include <unordered_set>

namespace bitxorcore { namespace utils {

	struct ShortHash_tag {};
	using ShortHash = BaseValue<uint32_t, ShortHash_tag>;

	// Trivial hash function for ShortHash
	struct ShortHashHasher {
		size_t operator()(const ShortHash& shortHash) const {
			return shortHash.unwrap();
		}
	};

	/// Unordered set of short hashes.
	using ShortHashesSet = std::unordered_set<ShortHash, ShortHashHasher>;

	/// Gets the short hash corresponding to \a hash.
	inline ShortHash ToShortHash(const Hash256& hash) {
		return reinterpret_cast<const ShortHash&>(hash[0]);
	}
}}
