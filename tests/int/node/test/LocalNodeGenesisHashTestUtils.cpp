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

#include "LocalNodeGenesisHashTestUtils.h"
#include "plugins/txes/token/src/model/TokenEntityType.h"
#include "bitxorcore/model/BlockStatementBuilder.h"
#include "bitxorcore/model/BlockUtils.h"
#include "tests/test/core/StorageTestUtils.h"
#include "tests/test/core/mocks/MockMemoryBlockStorage.h"
#include "tests/test/nodeps/TestConstants.h"

namespace bitxorcore { namespace test {

	namespace {
		std::vector<uint32_t> GetTokenSupplyChangePrimarySourceIds(const model::Block& block) {
			uint32_t transactionIndex = 0;
			std::vector<uint32_t> primarySourceIds;
			for (const auto& transaction : block.Transactions()) {
				if (model::Entity_Type_Token_Supply_Change == transaction.Type)
					primarySourceIds.push_back(transactionIndex + 1); // transaction sources are 1-based

				++transactionIndex;
			}

			if (2u != primarySourceIds.size())
				BITXORCORE_THROW_INVALID_ARGUMENT_1("genesis block has unexpected number of token supply changes", primarySourceIds.size());

			return primarySourceIds;
		}
	}

	void SetGenesisReceiptsHash(const std::string& destination, GenesisStorageDisposition disposition) {
		// calculate the receipts hash (default genesis block has zeroed receipts hash)
		auto modify = GenesisStorageDisposition::Seed == disposition ? ModifySeedGenesis : ModifyGenesis;
		modify(destination, [](auto& genesisBlock, const auto&) {
			model::BlockStatementBuilder blockStatementBuilder;

			// 1. add harvest fee receipt
			auto totalFee = model::CalculateBlockTransactionsInfo(genesisBlock).TotalFee;

			auto feeTokenId = Default_Currency_Token_Id;
			model::BalanceChangeReceipt receipt(
					model::Receipt_Type_Harvest_Fee,
					model::GetSignerAddress(genesisBlock),
					feeTokenId,
					totalFee);
			blockStatementBuilder.addReceipt(receipt);

			// 2. add token aliases (supply tx first uses alias, block token order is deterministic)
			auto aliasFirstUsedPrimarySourceIds = GetTokenSupplyChangePrimarySourceIds(genesisBlock);
			blockStatementBuilder.setSource({ aliasFirstUsedPrimarySourceIds[0], 0 });
			blockStatementBuilder.addResolution(UnresolvedTokenId(0x85BBEA6CC462B244), feeTokenId);

			blockStatementBuilder.setSource({ aliasFirstUsedPrimarySourceIds[1], 0 });
			blockStatementBuilder.addResolution(UnresolvedTokenId(0x941299B2B7E1291C), Default_Harvesting_Token_Id);

			// 3. calculate the block receipts hash
			auto pStatement = blockStatementBuilder.build();
			genesisBlock.ReceiptsHash = model::CalculateMerkleHash(*pStatement);
		});
	}

	void SetGenesisStateHash(
			const std::string& destination,
			GenesisStorageDisposition disposition,
			const config::BitxorCoreConfiguration& config) {
		// calculate the state hash (default genesis block has zeroed state hash)
		auto modify = GenesisStorageDisposition::Seed == disposition ? ModifySeedGenesis : ModifyGenesis;
		modify(destination, [&config](auto& genesisBlock, const auto& genesisBlockElement) {
			genesisBlock.StateHash = CalculateGenesisStateHash(genesisBlockElement, config);
		});
	}
}}
