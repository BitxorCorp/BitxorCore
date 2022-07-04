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
#include "HarvesterBlockGenerator.h"
#include "UnlockedAccounts.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/model/Elements.h"
#include "bitxorcore/model/EntityInfo.h"

namespace bitxorcore { namespace harvesting { struct BlockExecutionHashes; } }

namespace bitxorcore { namespace harvesting {

	/// Harvests new blocks.
	class Harvester {
	public:
		/// Creates a harvester around bitxorcore \a cache, blockchain \a config, \a beneficiary,
		/// unlocked accounts set (\a unlockedAccounts) and \a blockGenerator used to customize block generation.
		Harvester(
				const cache::BitxorCoreCache& cache,
				const model::BlockchainConfiguration& config,
				const Address& beneficiary,
				const UnlockedAccounts& unlockedAccounts,
				const BlockGenerator& blockGenerator);

	public:
		/// Creates the best block (if any) harvested by any unlocked account.
		/// Created block will have \a lastBlockElement as parent and \a timestamp as timestamp.
		std::unique_ptr<model::Block> harvest(const model::BlockElement& lastBlockElement, Timestamp timestamp);

	private:
		const cache::BitxorCoreCache& m_cache;
		const model::BlockchainConfiguration m_config;
		const Address m_beneficiary;
		const UnlockedAccounts& m_unlockedAccounts;
		BlockGenerator m_blockGenerator;
	};
}}
