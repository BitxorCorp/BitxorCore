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
#include "TransactionTestUtils.h"
#include "bitxorcore/model/Elements.h"
#include "bitxorcore/model/RangeTypes.h"
#include <list>
#include <memory>
#include <vector>

namespace bitxorcore { namespace test {

	/// Hash string of the deterministic block.
	constexpr auto Deterministic_Block_Hash_String = "D92AB24A3D09D10870F4E2DCFE9A6CA7EDE5A79CD492F9D7FC9A071ABC3C0F5A";

	// region TestBlockTransactions

	/// Container of transactions for seeding a test block.
	class TestBlockTransactions {
	public:
		/// Creates block transactions from const \a transactions.
		TestBlockTransactions(const ConstTransactions& transactions);

		/// Creates block transactions from mutable \a transactions.
		TestBlockTransactions(const MutableTransactions& transactions);

		/// Creates \a numTransactions (random) block transactions.
		TestBlockTransactions(size_t numTransactions);

	public:
		/// Gets the transactions.
		const ConstTransactions& get() const;

	private:
		ConstTransactions m_transactions;
	};

	// endregion

	// region Block factory functions

	/// Generates an empty block with random signer and no transactions.
	std::unique_ptr<model::Block> GenerateEmptyRandomBlock();

	/// Generates an importance block with random signer and \a transactions.
	std::unique_ptr<model::Block> GenerateImportanceBlockWithTransactions(const TestBlockTransactions& transactions);

	/// Generates a block with random signer and \a transactions.
	std::unique_ptr<model::Block> GenerateBlockWithTransactions(const TestBlockTransactions& transactions);

	/// Generates a block with a given \a signer and \a transactions.
	std::unique_ptr<model::Block> GenerateBlockWithTransactions(const crypto::KeyPair& signer, const TestBlockTransactions& transactions);

	/// Generates a block with \a numTransactions transactions at \a height.
	std::unique_ptr<model::Block> GenerateBlockWithTransactions(size_t numTransactions, Height height);

	/// Generates a block with \a numTransactions transactions at \a height and \a timestamp.
	std::unique_ptr<model::Block> GenerateBlockWithTransactions(size_t numTransactions, Height height, Timestamp timestamp);

	/// Generates a predefined block, i.e. this function will always return the same block.
	std::unique_ptr<model::Block> GenerateDeterministicBlock();

	// endregion

	/// Creates a buffer containing \a numBlocks random blocks (all with no transactions).
	std::vector<uint8_t> CreateRandomBlockBuffer(size_t numBlocks);

	/// Copies \a blocks into an entity range.
	model::BlockRange CreateEntityRange(const std::vector<const model::Block*>& blocks);

	/// Creates a block entity range composed of \a numBlocks blocks.
	model::BlockRange CreateBlockEntityRange(size_t numBlocks);

	/// Creates \a count ranges of blocks with \a increment additional blocks in each range.
	std::vector<model::BlockRange> PrepareRanges(size_t count, size_t increment = 0);

	/// Counts the number of transactions in \a block.
	size_t CountTransactions(const model::Block& block);

	/// Converts \a block to a block element.
	model::BlockElement BlockToBlockElement(const model::Block& block);

	/// Converts \a block to a block element with specified generation hash seed (\a generationHashSeed).
	model::BlockElement BlockToBlockElement(const model::Block& block, const GenerationHashSeed& generationHashSeed);

	/// Converts \a block with \a hash to a block element.
	model::BlockElement BlockToBlockElement(const model::Block& block, const Hash256& hash);

	/// Verifies that block elements \a expectedBlockElement and \a blockElement are equivalent.
	void AssertEqual(const model::BlockElement& expectedBlockElement, const model::BlockElement& blockElement);
}}
