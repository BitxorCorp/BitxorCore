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
#include "bitxorcore/model/HeightHashPair.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/subscribers/FinalizationSubscriber.h"

namespace bitxorcore {
	namespace io {
		class BlockStorageCache;
		class PrevoteChainStorage;
	}
}

namespace bitxorcore { namespace chain {

	/// Finalization subscriber that patches the local chain with a prevoted chain when the prevoted chain but not the local chain
	/// contains the finalized block.
	class FinalizationPatchingSubscriber : public subscribers::FinalizationSubscriber {
	public:
		/// Creates a subscriber around \a prevoteChainStorage, \a blockStorage and \a blockRangeConsumer.
		FinalizationPatchingSubscriber(
				io::PrevoteChainStorage& prevoteChainStorage,
				const io::BlockStorageCache& blockStorage,
				const consumer<model::BlockRange&&>& blockRangeConsumer);

	public:
		void notifyFinalizedBlock(const model::FinalizationRound& round, Height height, const Hash256& hash) override;

	private:
		io::PrevoteChainStorage& m_prevoteChainStorage;
		const io::BlockStorageCache& m_blockStorage;
		consumer<model::BlockRange&&> m_blockRangeConsumer;
	};
}}
