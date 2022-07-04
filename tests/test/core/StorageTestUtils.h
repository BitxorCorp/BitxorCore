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
#include "bitxorcore/functions.h"
#include <string>

namespace bitxorcore {
	namespace model {
		struct Block;
		struct BlockElement;
	}
}

namespace bitxorcore { namespace test {

	/// Prepares the storage by creating the \a destination directory structure and seeding a genesis block.
	void PrepareStorage(const std::string& destination);

	/// Prepares the seed storage by creating the \a destination directory structure and seeding a genesis block.
	void PrepareSeedStorage(const std::string& destination);

	/// Prepares the storage by creating the \a destination directory structure.
	void PrepareStorageWithoutGenesis(const std::string& destination);

	/// Modifies the genesis block stored in \a destination by applying \a modify.
	void ModifyGenesis(const std::string& destination, const consumer<model::Block&, const model::BlockElement&>& modify);

	/// Modifies the seed genesis block stored in \a destination by applying \a modify.
	void ModifySeedGenesis(const std::string& destination, const consumer<model::Block&, const model::BlockElement&>& modify);

	/// Fakes file-based chain located at \a destination to \a height
	/// by setting proper value in index.dat and filling 00000/hashes.dat.
	void FakeHeight(const std::string& destination, uint64_t height);
}}
