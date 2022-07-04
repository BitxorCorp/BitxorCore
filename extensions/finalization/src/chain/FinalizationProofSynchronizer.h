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
#include "bitxorcore/chain/RemoteNodeSynchronizer.h"
#include "bitxorcore/types.h"

namespace bitxorcore {
	namespace api { class RemoteProofApi; }
	namespace io {
		class BlockStorageCache;
		class ProofStorageCache;
	}
	namespace model { struct FinalizationProof; }
}

namespace bitxorcore { namespace chain {

	/// Creates a finalization proof synchronizer around block storage (\a blockStorage) and proof storage (\a proofStorage)
	/// given \a votingSetGrouping, \a unfinalizedBlocksDuration and \a proofValidator.
	RemoteNodeSynchronizer<api::RemoteProofApi> CreateFinalizationProofSynchronizer(
			uint64_t votingSetGrouping,
			BlockDuration unfinalizedBlocksDuration,
			const io::BlockStorageCache& blockStorage,
			io::ProofStorageCache& proofStorage,
			const predicate<const model::FinalizationProof&>& proofValidator);
}}
