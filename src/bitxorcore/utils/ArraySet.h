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
#include "Hashers.h"
#include "bitxorcore/types.h"
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace bitxorcore { namespace utils {

	/// Functor for hashing an array pointer.
	template<typename TArray>
	struct ArrayPointerHasher {
		size_t operator()(const TArray* pArray) const {
			return ArrayHasher<TArray>()(*pArray);
		}
	};

	/// Functor for comparing two array pointers.
	template<typename TArray>
	struct ArrayPointerEquality {
		bool operator()(const TArray* pLhs, const TArray* pRhs) const {
			return *pLhs == *pRhs;
		}
	};

	/// Unordered set of arrays.
	template<typename TArray>
	using ArraySet = std::unordered_set<TArray, ArrayHasher<TArray>>;

	/// Unordered set of array pointers.
	template<typename TArray>
	using ArrayPointerSet = std::unordered_set<const TArray*, ArrayPointerHasher<TArray>, ArrayPointerEquality<TArray>>;

	/// Unordered map of array pointers to flags.
	template<typename TArray>
	using ArrayPointerFlagMap = std::unordered_map<const TArray*, bool, ArrayPointerHasher<TArray>, ArrayPointerEquality<TArray>>;

	// region well known array sets

	/// Hash set.
	using HashSet = ArraySet<Hash256>;

	/// Key set.
	using KeySet = ArraySet<Key>;

	/// Sorted key set.
	using SortedKeySet = std::set<Key>;

	/// Hash pointer set.
	using HashPointerSet = ArrayPointerSet<Hash256>;

	/// Key pointer set.
	using KeyPointerSet = ArrayPointerSet<Key>;

	// endregion
}}
