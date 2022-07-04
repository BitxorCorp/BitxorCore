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

#include "bitxorcore/local/recovery/RepairImportance.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/RawFile.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace local {

#define TEST_CLASS RepairImportanceTests

	namespace {
		void RunRepairImportanceTest(consumers::CommitOperationStep commitStep, size_t numExpectedFiles) {
			// Arrange:
			test::TempDirectoryGuard tempDir;
			auto importanceDirectory = std::filesystem::path(tempDir.name()) / "importance";
			auto importanceWipDirectory = importanceDirectory / "wip";
			std::filesystem::create_directories(importanceWipDirectory);

			// - write two files to wip
			for (auto name : { "foo", "bar" })
				io::RawFile((importanceWipDirectory / name).generic_string(), io::OpenMode::Read_Write);

			// Sanity:
			EXPECT_EQ(2u, test::CountFilesAndDirectories(importanceWipDirectory));

			// Act:
			RepairImportance(config::BitxorCoreDataDirectory(tempDir.name()), commitStep);

			// Assert:
			EXPECT_EQ(1u + numExpectedFiles, test::CountFilesAndDirectories(importanceDirectory));
			EXPECT_EQ(0u, test::CountFilesAndDirectories(importanceWipDirectory));
		}
	}

	TEST(TEST_CLASS, RepairImportancePurgesWipWhenBlocksWritten) {
		RunRepairImportanceTest(consumers::CommitOperationStep::Blocks_Written, 0);
	}

	TEST(TEST_CLASS, RepairImportancePurgesWipWhenAllUpdated) {
		RunRepairImportanceTest(consumers::CommitOperationStep::All_Updated, 0);
	}

	TEST(TEST_CLASS, RepairImportanceCommitsWipWhenStateWritten) {
		RunRepairImportanceTest(consumers::CommitOperationStep::State_Written, 2);
	}
}}
