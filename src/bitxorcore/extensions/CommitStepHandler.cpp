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

#include "CommitStepHandler.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/IndexFile.h"

namespace bitxorcore { namespace extensions {

	consumers::BlockchainSyncHandlers::CommitStepFunc CreateCommitStepHandler(const config::BitxorCoreDataDirectory& dataDirectory) {
		return [dataDirectory](auto step) {
			io::IndexFile(dataDirectory.rootDir().file("commit_step.dat")).set(utils::to_underlying_type(step));

			if (consumers::CommitOperationStep::All_Updated != step)
				return;

			auto stateChangeDirectory = dataDirectory.spoolDir("state_change");
			auto syncIndexWriterFile = io::IndexFile(stateChangeDirectory.file("index_server.dat"));
			if (!syncIndexWriterFile.exists())
				return;

			io::IndexFile(stateChangeDirectory.file("index.dat")).set(syncIndexWriterFile.get());
		};
	}
}}
