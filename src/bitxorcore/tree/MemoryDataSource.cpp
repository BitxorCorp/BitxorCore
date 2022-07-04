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

#include "MemoryDataSource.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace tree {

	MemoryDataSource::MemoryDataSource(DataSourceVerbosity verbosity) : m_isVerbose(DataSourceVerbosity::Verbose == verbosity)
	{}

	size_t MemoryDataSource::size() const {
		return m_leafNodes.size() + m_branchNodes.size();
	}

	TreeNode MemoryDataSource::get(const Hash256& hash) const {
		auto leafNodeIter = m_leafNodes.find(hash);
		if (m_leafNodes.cend() != leafNodeIter)
			return TreeNode(leafNodeIter->second);

		auto branchNodeIter = m_branchNodes.find(hash);
		if (m_branchNodes.cend() != branchNodeIter)
			return TreeNode(branchNodeIter->second);

		return TreeNode();
	}

	void MemoryDataSource::forEach(const consumer<const TreeNode&>& consumer) const {
		for (const auto& pair : m_leafNodes)
			consumer(TreeNode(pair.second));

		for (const auto& pair : m_branchNodes)
			consumer(TreeNode(pair.second));
	}

	void MemoryDataSource::set(const LeafTreeNode& node) {
		if (m_isVerbose) {
			BITXORCORE_LOG(debug)
					<< "saving leaf node: " << node.path() << ", hash = " << node.hash()
					<< ", value = " << node.value();
		}

		m_leafNodes.emplace(node.hash(), node);
	}

	void MemoryDataSource::set(const BranchTreeNode& node) {
		if (m_isVerbose) {
			BITXORCORE_LOG(debug)
					<< "saving branch node: " << node.path() << ", hash = " << node.hash()
					<< ", #links " << node.numLinks();
		}

		m_branchNodes.emplace(node.hash(), node);
	}

	void MemoryDataSource::clear() {
		m_leafNodes.clear();
		m_branchNodes.clear();
	}
}}
