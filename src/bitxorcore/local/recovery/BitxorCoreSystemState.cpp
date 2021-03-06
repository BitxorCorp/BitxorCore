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

#include "BitxorCoreSystemState.h"
#include "bitxorcore/io/IndexFile.h"

namespace bitxorcore { namespace local {

	namespace {
		constexpr auto Broker_Lock_Filename = "broker.lock";
		constexpr auto Server_Lock_Filename = "server.lock";

		// this file can always be deleted because, on success, it is moved to harvesters.dat
		// it will only ever exist when there is a failure (and it was created but something happened before it was moved)
		constexpr auto Harvesters_Temp_Filename = "harvesters.dat.tmp";

		constexpr auto Commit_Step_Filename = "commit_step.dat";
	}

	BitxorCoreSystemState::BitxorCoreSystemState(const config::BitxorCoreDataDirectory& dataDirectory) : m_dataDirectory(dataDirectory)
	{}

	bool BitxorCoreSystemState::shouldRecoverBroker() const {
		return std::filesystem::exists(qualifyRootFile(Broker_Lock_Filename));
	}

	bool BitxorCoreSystemState::shouldRecoverServer() const {
		return std::filesystem::exists(qualifyRootFile(Server_Lock_Filename));
	}

	consumers::CommitOperationStep BitxorCoreSystemState::commitStep() const {
		io::IndexFile indexFile(qualifyRootFile(Commit_Step_Filename));
		return indexFile.exists()
				? static_cast<consumers::CommitOperationStep>(indexFile.get())
				: consumers::CommitOperationStep::All_Updated;
	}

	void BitxorCoreSystemState::reset() {
		for (const auto& filename : { Broker_Lock_Filename, Server_Lock_Filename, Harvesters_Temp_Filename, Commit_Step_Filename })
			std::filesystem::remove(qualifyRootFile(filename));
	}

	std::string BitxorCoreSystemState::qualifyRootFile(const std::string& filename) const {
		return m_dataDirectory.rootDir().file(filename);
	}
}}
