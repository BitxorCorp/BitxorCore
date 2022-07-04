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
#include "ResolutionStatement.h"
#include "TransactionStatement.h"
#include <map>

namespace bitxorcore { namespace model {

	/// Collection of statements scoped to a block.
	struct BlockStatement {
		/// Transaction statements.
		std::map<ReceiptSource, TransactionStatement> TransactionStatements;

		/// Address resolution statements.
		std::map<UnresolvedAddress, AddressResolutionStatement> AddressResolutionStatements;

		/// Token resolution statements.
		std::map<UnresolvedTokenId, TokenResolutionStatement> TokenResolutionStatements;
	};

	/// Calculates the merkle hash for \a statement.
	Hash256 CalculateMerkleHash(const BlockStatement& statement);

	/// Calculates the merkle tree for \a statement.
	std::vector<Hash256> CalculateMerkleTree(const BlockStatement& statement);

	/// Counts the total number of statements in \a statement.
	size_t CountTotalStatements(const BlockStatement& statement);

	/// Creates a deep copy of \a source into \a destination.
	void DeepCopyTo(BlockStatement& destination, const BlockStatement& source);

	/// Creates a deep copy of \a source into \a destination excluding receipts with primary source id greater than \a maxSourcePrimaryId.
	void DeepCopyTo(BlockStatement& destination, const BlockStatement& source, uint32_t maxSourcePrimaryId);
}}
