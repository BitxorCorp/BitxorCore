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

#include "RepairImportance.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/FilesystemUtils.h"

namespace bitxorcore { namespace local {

	void RepairImportance(const config::BitxorCoreDataDirectory& dataDirectory, consumers::CommitOperationStep commitStep) {
		auto importanceDirectory = dataDirectory.dir("importance");
		auto importanceWipDirectory = importanceDirectory.dir("wip");
		if (consumers::CommitOperationStep::State_Written != commitStep) {
			// importance/wip files should be purged if state hasn't been fully written
			BITXORCORE_LOG(debug) << " - purging " << importanceWipDirectory.str();
			io::PurgeDirectory(importanceWipDirectory.str());
			return;
		} else {
			// otherwise, copy importance/wip files to commit them
			BITXORCORE_LOG(debug) << " - committing " << importanceWipDirectory.str();
			io::MoveAllFiles(importanceWipDirectory.str(), importanceDirectory.str());
		}
	}
}}
