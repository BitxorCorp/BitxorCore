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

#include "GenesisExecutionHasher.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/model/Elements.h"

namespace bitxorcore { namespace tools { namespace bxorgen {

	namespace {
		std::string Format(const BlockExecutionHashesInfo& blockExecutionHashesInfo) {
			std::ostringstream out;
			out
					<< "       State Hash: " << blockExecutionHashesInfo.StateHash << std::endl
					<< "--- Components (" << blockExecutionHashesInfo.SubCacheMerkleRoots.size() << ") ---" << std::endl;

			for (const auto& subCacheMerkleRoot : blockExecutionHashesInfo.SubCacheMerkleRoots)
				out << " + " << subCacheMerkleRoot << std::endl;

			return out.str();
		}
	}

	GenesisExecutionHashesDescriptor CalculateAndLogGenesisExecutionHashes(
			const model::BlockElement& blockElement,
			const config::BitxorCoreConfiguration& config,
			plugins::PluginManager& pluginManager) {
		if (!config.Node.EnableCacheDatabaseStorage || !config.Blockchain.EnableVerifiableState)
			BITXORCORE_LOG(warning) << "cache database storage and verifiable state must both be enabled to calculate state hash";

		BITXORCORE_LOG(info) << "calculating genesis state hash";
		auto blockExecutionHashesInfo = CalculateGenesisBlockExecutionHashes(blockElement, config.Blockchain, pluginManager);
		std::ostringstream out;
		out
				<< "           Height: " << blockElement.Block.Height << std::endl
				<< " Harvesting Count: " << blockExecutionHashesInfo.HarvestingEligibleAccountsCount << std::endl
				<< "     Voting Count: " << blockExecutionHashesInfo.VotingEligibleAccountsCount << std::endl
				<< "   Voting Balance: " << blockExecutionHashesInfo.TotalVotingBalance << std::endl
				<< "*" << std::endl
				<< "  Generation Hash: " << blockElement.GenerationHash << std::endl
				<< "Transactions Hash: " << blockElement.Block.TransactionsHash << std::endl
				<< "    Receipts Hash: " << blockExecutionHashesInfo.ReceiptsHash << std::endl
				<< Format(blockExecutionHashesInfo);

		GenesisExecutionHashesDescriptor descriptor;
		descriptor.VotingEligibleAccountsCount = blockExecutionHashesInfo.VotingEligibleAccountsCount;
		descriptor.HarvestingEligibleAccountsCount = blockExecutionHashesInfo.HarvestingEligibleAccountsCount;
		descriptor.TotalVotingBalance = blockExecutionHashesInfo.TotalVotingBalance;

		descriptor.ReceiptsHash = blockExecutionHashesInfo.ReceiptsHash;
		descriptor.StateHash = blockExecutionHashesInfo.StateHash;
		descriptor.SubCacheMerkleRoots = blockExecutionHashesInfo.SubCacheMerkleRoots;
		descriptor.pBlockStatement = std::move(blockExecutionHashesInfo.pBlockStatement);
		descriptor.Summary = out.str();
		return descriptor;
	}
}}}
