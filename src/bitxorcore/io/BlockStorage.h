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
#include "bitxorcore/model/Elements.h"
#include "bitxorcore/model/EntityInfo.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/utils/NonCopyable.h"
#include <memory>

namespace bitxorcore { namespace io {

	/// Minimalistic interface for block storage (does not allow block loading).
	class LightBlockStorage : public utils::NonCopyable {
	public:
		virtual ~LightBlockStorage() = default;

	public:
		/// Gets the number of blocks.
		virtual Height chainHeight() const = 0;

		/// Gets a range of at most \a maxHashes hashes starting at \a height.
		virtual model::HashRange loadHashesFrom(Height height, size_t maxHashes) const = 0;

		/// Saves \a blockElement.
		virtual void saveBlock(const model::BlockElement& blockElement) = 0;

		/// Drops all blocks after \a height.
		virtual void dropBlocksAfter(Height height) = 0;
	};

	/// Interface for saving and loading blocks.
	class BlockStorage : public LightBlockStorage {
	public:
		/// Gets the block at \a height.
		virtual std::shared_ptr<const model::Block> loadBlock(Height height) const = 0;

		/// Gets the block element (owning a block) at \a height.
		virtual std::shared_ptr<const model::BlockElement> loadBlockElement(Height height) const = 0;

		/// Gets the optional block statement data at \a height.
		virtual std::pair<std::vector<uint8_t>, bool> loadBlockStatementData(Height height) const = 0;
	};

	/// Interface that allows saving, loading and pruning blocks.
	class PLUGIN_API_DEPENDENCY PrunableBlockStorage : public BlockStorage {
	public:
		/// Purges all blocks from storage.
		virtual void purge() = 0;
	};
}}
