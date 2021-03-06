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
#include "bitxorcore/model/Block.h"
#include "bitxorcore/model/TransactionSelectionStrategy.h"

namespace bitxorcore {
	namespace cache { class ReadWriteUtCache; }
	namespace harvesting { class HarvestingUtFacadeFactory; }
}

namespace bitxorcore { namespace harvesting {

	/// Generates a block from a seed block header given a maximum number of transactions.
	using BlockGenerator = std::function<std::unique_ptr<model::Block> (const model::BlockHeader&, uint32_t)>;

	/// Creates a default block generator around \a transactionRegistry, \a utFacadeFactory and \a utCache
	/// for specified transaction \a strategy.
	BlockGenerator CreateHarvesterBlockGenerator(
			model::TransactionSelectionStrategy strategy,
			const model::TransactionRegistry& transactionRegistry,
			const HarvestingUtFacadeFactory& utFacadeFactory,
			const cache::ReadWriteUtCache& utCache);
}}
