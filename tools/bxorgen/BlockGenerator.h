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
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/model/Elements.h"
#include <memory>

namespace bitxorcore {
	namespace model { class TransactionRegistry; }
	namespace tools {
		namespace bxorgen {
			struct GenesisConfiguration;
			struct GenesisExecutionHashesDescriptor;
		}
	}
}

namespace bitxorcore { namespace tools { namespace bxorgen {

	/// Creates a genesis block containing the specified additional transactions (\a additionalTransactions) given \a config.
	std::unique_ptr<model::Block> CreateGenesisBlock(const GenesisConfiguration& config, model::Transactions&& additionalTransactions);

	/// Updates a genesis \a block given \a config and \a executionHashesDescriptor.
	Hash256 UpdateGenesisBlock(
			const GenesisConfiguration& config,
			model::Block& block,
			GenesisExecutionHashesDescriptor& executionHashesDescriptor);

	/// Wraps a block element around \a block given \a config and \a transactionRegistry.
	model::BlockElement CreateGenesisBlockElement(
			const GenesisConfiguration& config,
			const model::TransactionRegistry& transactionRegistry,
			const model::Block& block);
}}}
