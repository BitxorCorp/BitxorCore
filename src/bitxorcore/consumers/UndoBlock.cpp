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

#include "UndoBlock.h"
#include "bitxorcore/chain/BlockExecutor.h"

namespace bitxorcore { namespace consumers {

	void UndoBlock(
			const model::BlockElement& blockElement,
			const chain::BlockExecutionContext& executionContext,
			UndoBlockType undoBlockType) {
		switch (undoBlockType) {
			case UndoBlockType::Rollback:
				// always rollback individual blocks because rocks state storage is independent of rocks tree storage
				BITXORCORE_LOG(debug) << "rolling back block at height " << blockElement.Block.Height;
				chain::RollbackBlock(blockElement, executionContext);
				break;

			case UndoBlockType::Common:
				if (!blockElement.SubCacheMerkleRoots.empty()) {
					// reset merkle roots when enabled (this is required to properly handle pruned, expired state entries)
					BITXORCORE_LOG(debug) << "rolling back state hash to height " << blockElement.Block.Height;
					executionContext.State.Cache.setSubCacheMerkleRoots(blockElement.SubCacheMerkleRoots);
				}
				break;
		}
	}
}}
