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
#include "Node.h"

namespace bitxorcore { namespace ionet {

	/// Equality object for Node.
	struct NodeEquality {
		/// Returns \c true if \a lhs and \a rhs are equal.
		bool operator()(const Node& lhs, const Node& rhs) const {
			return model::NodeIdentityEquality(model::NodeIdentityEqualityStrategy::Key_And_Host)(lhs.identity(), rhs.identity());
		}
	};

	/// Hasher object for Node.
	struct NodeHasher {
		/// Hashes \a node.
		size_t operator()(const Node& node) const {
			return model::NodeIdentityHasher(model::NodeIdentityEqualityStrategy::Key_And_Host)(node.identity());
		}
	};

	/// Unordered set of nodes.
	using NodeSet = std::unordered_set<Node, NodeHasher, NodeEquality>;
}}
