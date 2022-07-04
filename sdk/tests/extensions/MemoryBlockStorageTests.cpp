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

#include "src/extensions/MemoryBlockStorage.h"
#include "tests/test/core/BlockStorageTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS MemoryBlockStorageTests

	namespace {
		struct MemoryTraits {
			struct Guard {
				std::string name() const {
					return std::string();
				}
			};
			using StorageType = MemoryBlockStorage;

			static std::unique_ptr<StorageType> OpenStorage(const std::string&) {
				// load and copy the genesis into storage
				auto genesisBlockElement = test::BlockToBlockElement(test::GetGenesisBlock(), test::GetGenesisGenerationHashSeed());
				return std::make_unique<StorageType>(genesisBlockElement);
			}

			static std::unique_ptr<StorageType> PrepareStorage(const std::string& destination, Height height = Height()) {
				auto pStorage = OpenStorage(destination);

				// set storage height to `height - 1` because next block saved will be at `height`
				if (Height() != height)
					pStorage->dropBlocksAfter(height - Height(1));

				return pStorage;
			}
		};
	}

	DEFINE_BLOCK_STORAGE_TESTS(MemoryTraits)
	DEFINE_PRUNABLE_BLOCK_STORAGE_TESTS(MemoryTraits)
}}
