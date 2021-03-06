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

#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/RawFile.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace config {

#define TEST_CLASS BitxorCoreDataDirectoryTests

	namespace fs = std::filesystem;

	namespace {
		struct TestKey_tag {};
		using TestKey = utils::BaseValue<uint32_t, TestKey_tag>;

		auto Concat(const fs::path& baseDirectory, const fs::path& subDirectory) {
			return baseDirectory / subDirectory;
		}

		void AssertPermissions(const fs::path& fullPath) {
			auto status = fs::status(fullPath);
			EXPECT_NE(fs::perms::unknown, status.permissions());

			// permissions are not set on windows
#ifndef _WIN32
			EXPECT_EQ(fs::perms::owner_all, status.permissions() & fs::perms::all);
#endif
		}
	}

	// region BitxorCoreDirectory - basic

	TEST(TEST_CLASS, CanCreateBitxorCoreDirectory) {
		// Act:
		BitxorCoreDirectory directory("foo/bar");

		// Assert:
		EXPECT_EQ("foo/bar", directory.str());
		EXPECT_EQ(fs::path("foo/bar"), directory.path());
		EXPECT_EQ("foo/bar/abc", directory.file("abc"));
		EXPECT_EQ("foo/bar/def", directory.dir("def").str());

		EXPECT_FALSE(directory.exists());
	}

	TEST(TEST_CLASS, ExistsReturnsFalseWhenDirectoryDoesNotExists) {
		// Arrange:
		BitxorCoreDirectory directory("foo");

		// Act + Assert:
		EXPECT_FALSE(directory.exists());
	}

	TEST(TEST_CLASS, ExistsReturnsTrueWhenDirectoryExists) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		BitxorCoreDirectory directory(tempDir.name());

		// Act + Assert:
		EXPECT_TRUE(directory.exists());
	}

	// endregion

	// region BitxorCoreDirectory - filesystem

	namespace {
		struct CreateTraits {
			static void Create(const BitxorCoreDirectory& directory) {
				directory.create();
			}
		};

		struct CreateAllTraits {
			static void Create(const BitxorCoreDirectory& directory) {
				directory.createAll();
			}
		};
	}

#define CREATE_DIRECTORY_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, BitxorCoreDirectoryCreate_##TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<CreateTraits>(); } \
	TEST(TEST_CLASS, BitxorCoreDirectoryCreateAll_##TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<CreateAllTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	CREATE_DIRECTORY_TEST(CanCreateSingleDirectory_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto fullPath = Concat(tempDir.name(), "foobar");
		BitxorCoreDirectory dataDirectory(fullPath);

		// Sanity:
		EXPECT_FALSE(dataDirectory.exists());

		// Act:
		TTraits::Create(dataDirectory);

		// Assert:
		EXPECT_TRUE(dataDirectory.exists());
		AssertPermissions(fullPath);
	}

	CREATE_DIRECTORY_TEST(SucceedsWhenDirectoryAlreadyExists_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto fullPath = Concat(tempDir.name(), "foobar");
		BitxorCoreDirectory dataDirectory(fullPath);

		TTraits::Create(dataDirectory);

		// Sanity:
		EXPECT_TRUE(dataDirectory.exists());

		// Act:
		TTraits::Create(dataDirectory);

		// Assert:
		EXPECT_TRUE(dataDirectory.exists());
		AssertPermissions(fullPath);
	}

	CREATE_DIRECTORY_TEST(ThrowsWhenCollidingFileExists_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto fullPath = Concat(tempDir.name(), "foo");
		BitxorCoreDirectory dataDirectory(fullPath);

		io::RawFile file(fullPath.generic_string(), io::OpenMode::Read_Write);

		// Sanity:
		EXPECT_TRUE(fs::exists(fullPath));
		EXPECT_FALSE(dataDirectory.exists());

		// Act + Assert:
		EXPECT_THROW(TTraits::Create(dataDirectory), bitxorcore_runtime_error);
	}

	TEST(TEST_CLASS, BitxorCoreDirectoryCreate_ThrowsWhenDirectoryCannotBeCreated_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto fullPath = Concat(tempDir.name(), "foo") / "bar";
		BitxorCoreDirectory dataDirectory(fullPath);

		// Sanity:
		EXPECT_FALSE(dataDirectory.exists());

		// Act + Assert:
		EXPECT_THROW(BitxorCoreDirectory(fullPath).create(), bitxorcore_runtime_error);
	}

	TEST(TEST_CLASS, BitxorCoreDirectoryCreateAll_CanCreateMultipleDirectories_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto fullPath = Concat(tempDir.name(), "foo") / "baz" / "bar";
		BitxorCoreDirectory dataDirectory(fullPath);

		// Sanity:
		EXPECT_FALSE(dataDirectory.exists());

		// Act:
		BitxorCoreDirectory(fullPath).createAll();

		// Assert:
		EXPECT_TRUE(dataDirectory.exists());
		AssertPermissions(Concat(tempDir.name(), "foo"));
		AssertPermissions(Concat(tempDir.name(), "foo") / "baz");
		AssertPermissions(Concat(tempDir.name(), "foo") / "baz" / "bar");
	}

	TEST(TEST_CLASS, BitxorCoreDirectoryCreateAll_CanCreateMultipleDirectoriesWhenSomeAlreadyExist_Filesystem) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		auto partialPath = Concat(tempDir.name(), "foo") / "baz";
		BitxorCoreDirectory partialDirectory(partialPath);
		auto fullPath = partialPath / "bar" / "moon";
		BitxorCoreDirectory fullDirectory(fullPath);

		partialDirectory.createAll();

		// Sanity:
		EXPECT_TRUE(partialDirectory.exists());
		EXPECT_FALSE(fullDirectory.exists());

		// Act:
		fullDirectory.createAll();

		// Assert:
		EXPECT_TRUE(partialDirectory.exists());
		EXPECT_TRUE(fullDirectory.exists());
		AssertPermissions(Concat(tempDir.name(), "foo"));
		AssertPermissions(Concat(tempDir.name(), "foo") / "baz");
		AssertPermissions(Concat(tempDir.name(), "foo") / "baz" / "bar");
		AssertPermissions(Concat(tempDir.name(), "foo") / "baz" / "bar" / "moon");
	}

	// endregion

	// region BitxorCoreDataDirectory

	TEST(TEST_CLASS, CanGetRootDataDirectory) {
		// Arrange:
		BitxorCoreDataDirectory dataDirectory("foo");

		// Act:
		auto directory = dataDirectory.rootDir();

		// Assert:
		EXPECT_EQ("foo", directory.str());
		EXPECT_EQ(fs::path("foo"), directory.path());
		EXPECT_EQ("foo/abc", directory.file("abc"));
		EXPECT_EQ("foo/def", directory.dir("def").str());
	}

	TEST(TEST_CLASS, CanGetSubDirectory) {
		// Arrange:
		BitxorCoreDataDirectory dataDirectory("foo");

		// Act:
		auto directory = dataDirectory.dir("bar");

		// Assert:
		EXPECT_EQ("foo/bar", directory.str());
		EXPECT_EQ(fs::path("foo/bar"), directory.path());
		EXPECT_EQ("foo/bar/abc", directory.file("abc"));
		EXPECT_EQ("foo/bar/def", directory.dir("def").str());
	}

	TEST(TEST_CLASS, CanGetSpoolSubDirectory) {
		// Arrange:
		BitxorCoreDataDirectory dataDirectory("foo");

		// Act:
		auto directory = dataDirectory.spoolDir("bar");

		// Assert:
		EXPECT_EQ("foo/spool/bar", directory.str());
		EXPECT_EQ(fs::path("foo/spool/bar"), directory.path());
		EXPECT_EQ("foo/spool/bar/abc", directory.file("abc"));
		EXPECT_EQ("foo/spool/bar/def", directory.dir("def").str());
	}

	TEST(TEST_CLASS, CanGetStorageDirectory) {
		// Arrange:
		BitxorCoreDataDirectory dataDirectory("foo");

		// Act:
		auto storageDirectory = dataDirectory.storageDir(TestKey(Files_Per_Storage_Directory * 5 + 13));

		// Assert:
		auto subDirectoryPath = Concat("foo", "00005");
		EXPECT_EQ(subDirectoryPath.generic_string(), storageDirectory.str());
		EXPECT_EQ((subDirectoryPath / "00013:suffix").generic_string(), storageDirectory.storageFile(":suffix"));
		EXPECT_EQ((subDirectoryPath / ":suffix").generic_string(), storageDirectory.indexFile(std::string(), ":suffix"));
		EXPECT_EQ((subDirectoryPath / "prefix_:suffix").generic_string(), storageDirectory.indexFile("prefix_", ":suffix"));
	}

	// endregion

	// region BitxorCoreDataDirectoryPreparer

	namespace {
		constexpr const char* Sub_Directories[] = { "/importance", "/importance/wip", "/spool" };

		void AssertPreparedDirectoriesExist(const std::string& directory) {
			EXPECT_EQ(2u, test::CountFilesAndDirectories(directory));
			EXPECT_EQ(1u, test::CountFilesAndDirectories(directory + "/importance"));
			EXPECT_EQ(0u, test::CountFilesAndDirectories(directory + "/importance/wip"));
			EXPECT_EQ(0u, test::CountFilesAndDirectories(directory + "/spool"));
		}
	}

	TEST(TEST_CLASS, BitxorCoreDataDirectoryPreparer_PreparerCreatesSubDirectoriesWhenNotPresent) {
		// Arrange:
		test::TempDirectoryGuard tempDir;

		// Sanity:
		for (const auto* subDirectory : Sub_Directories)
			EXPECT_FALSE(fs::is_directory(tempDir.name() + subDirectory)) << subDirectory;

		// Act:
		auto dataDirectory = BitxorCoreDataDirectoryPreparer::Prepare(tempDir.name());

		// Assert:
		EXPECT_EQ(tempDir.name(), dataDirectory.rootDir().str());
		for (const auto* subDirectory : Sub_Directories)
			EXPECT_TRUE(fs::is_directory(tempDir.name() + subDirectory)) << subDirectory;

		AssertPreparedDirectoriesExist(tempDir.name());
	}

	TEST(TEST_CLASS, BitxorCoreDataDirectoryPreparer_PreparerSucceedsWhenSubDirectoriesArePresent) {
		// Arrange:
		test::TempDirectoryGuard tempDir;
		for (const auto* subDirectory : Sub_Directories)
			fs::create_directory(tempDir.name() + subDirectory);

		// Sanity:
		for (const auto* subDirectory : Sub_Directories)
			EXPECT_TRUE(fs::is_directory(tempDir.name() + subDirectory)) << subDirectory;

		// Act:
		auto dataDirectory = BitxorCoreDataDirectoryPreparer::Prepare(tempDir.name());

		// Assert:
		EXPECT_EQ(tempDir.name(), dataDirectory.rootDir().str());
		for (const auto* subDirectory : Sub_Directories)
			EXPECT_TRUE(fs::is_directory(tempDir.name() + subDirectory)) << subDirectory;

		AssertPreparedDirectoriesExist(tempDir.name());
	}

	// endregion

	// region BitxorCoreStorageDirectoryPreparer

	namespace {
		void AssertPrepareStorageCreatesSubDirectory(TestKey key, const std::string& expectedSubDirectoryName) {
			// Arrange:
			test::TempDirectoryGuard tempDir;

			// Sanity:
			EXPECT_FALSE(fs::exists(Concat(tempDir.name(), expectedSubDirectoryName)));

			// Act:
			auto storageDirectory = config::BitxorCoreStorageDirectoryPreparer::Prepare(tempDir.name(), key);

			// Assert:
			EXPECT_TRUE(fs::exists(Concat(tempDir.name(), expectedSubDirectoryName)));
		}
	}

	TEST(TEST_CLASS, BitxorCoreStorageDirectoryPreparer_PreparerCreatesDirectory) {
		AssertPrepareStorageCreatesSubDirectory(TestKey(5), "00000");
	}

	TEST(TEST_CLASS, BitxorCoreStorageDirectoryPreparer_PreparerCreatesDirectory_BeforeBoundary) {
		AssertPrepareStorageCreatesSubDirectory(TestKey(Files_Per_Storage_Directory - 1), "00000");
	}

	TEST(TEST_CLASS, BitxorCoreStorageDirectoryPreparer_PreparerCreatesDirectory_AtBoundary) {
		AssertPrepareStorageCreatesSubDirectory(TestKey(Files_Per_Storage_Directory), "00001");
	}

	TEST(TEST_CLASS, BitxorCoreStorageDirectoryPreparer_PreparerCreatesDirectory_AfterBoundary) {
		AssertPrepareStorageCreatesSubDirectory(TestKey(Files_Per_Storage_Directory + 1), "00001");
	}

	// endregion
}}
