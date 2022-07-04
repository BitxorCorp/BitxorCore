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
#include "sdk/src/extensions/MemoryBlockStorage.h"
#include "bitxorcore/io/BlockStorageCache.h"

namespace bitxorcore { namespace mocks {

	/// Mock memory-based block storage that loads and saves blocks in memory.
	class MockMemoryBlockStorage : public extensions::MemoryBlockStorage {
	public:
		/// Creates a mock memory-based block storage.
		MockMemoryBlockStorage();
	};

	/// Creates a memory based block storage composed of \a numBlocks.
	std::unique_ptr<io::PrunableBlockStorage> CreateMemoryBlockStorage(uint32_t numBlocks);

	/// Creates a memory based block storage cache composed of \a numBlocks.
	std::unique_ptr<io::BlockStorageCache> CreateMemoryBlockStorageCache(uint32_t numBlocks);

	/// Seeds \a storage with \a numBlocks.
	void SeedStorageWithFixedSizeBlocks(io::BlockStorageCache& storage, uint32_t numBlocks);
}}
