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

#include "StorageTestUtils.h"
#include "sdk/src/extensions/BlockExtensions.h"
#include "bitxorcore/io/FileBlockStorage.h"
#include "bitxorcore/io/PodIoUtils.h"
#include "bitxorcore/io/RawFile.h"
#include "bitxorcore/model/EntityHasher.h"
#include "tests/test/nodeps/Genesis.h"
#include "tests/test/nodeps/TestConstants.h"
#include "tests/test/nodeps/TestNetworkConstants.h"
#include <filesystem>

namespace bitxorcore { namespace test {

	namespace {
		constexpr auto Source_Directory = "../seed/testnet";

		void MakeReadOnly(const std::filesystem::path& filepath) {
			std::filesystem::permissions(filepath, std::filesystem::perms::owner_read, std::filesystem::perm_options::replace);
		}

		void SetIndexHeight(const std::string& destination, uint64_t height) {
			io::RawFile indexFile(destination + "/index.dat", io::OpenMode::Read_Write);
			io::Write64(indexFile, height);
		}

		void PrepareStorage(const std::string& destination, uint32_t fileDatabaseBatchSize) {
			PrepareStorageWithoutGenesis(destination);

			for (auto filename : { "proof.index.dat" })
				std::filesystem::copy_file(std::string(Source_Directory) + "/" + filename, destination + "/" + filename);

			for (auto filename : { "hashes.dat", "proof.heights.dat" })
				std::filesystem::copy_file(std::string(Source_Directory) + "/00000/" + filename, destination + "/00000/" + filename);

			for (auto extension : { ".dat", ".proof" }) {
				if (1 == fileDatabaseBatchSize) {
					std::filesystem::copy_file(
							std::string(Source_Directory) + "/00000/00001" + extension,
							destination + "/00000/00001" + extension);
					continue;
				}

				io::RawFile inputFile(std::string(Source_Directory) + "/00000/00001" + extension, io::OpenMode::Read_Only);
				io::RawFile outputFile(destination + "/00000/00000" + extension, io::OpenMode::Read_Write);

				// write file database header
				auto headerSize = fileDatabaseBatchSize * sizeof(uint64_t);
				outputFile.write(std::vector<uint8_t>(headerSize));
				outputFile.seek(sizeof(uint64_t));
				Write64(outputFile, headerSize);
				outputFile.seek(headerSize);

				// copy input file contents
				std::vector<uint8_t> inputBuffer(inputFile.size());
				inputFile.read(inputBuffer);
				outputFile.write(inputBuffer);
			}
		}
	}

	void PrepareStorage(const std::string& destination) {
		PrepareStorage(destination, File_Database_Batch_Size);
	}

	void PrepareSeedStorage(const std::string& destination) {
		PrepareStorage(destination, 1);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(destination)) {
			if (!entry.is_regular_file())
				continue;

			MakeReadOnly(entry.path());
		}
	}

	void PrepareStorageWithoutGenesis(const std::string& destination) {
		const std::string genesisDirectory = "/00000";
		std::filesystem::create_directories(destination + genesisDirectory);

		SetIndexHeight(destination, 1);
	}

	namespace {
		std::string GetGenesisFilename(const std::string& destination, uint32_t fileDatabaseBatchSize) {
			return destination + "/00000/0000" + (1 == fileDatabaseBatchSize ? "1.dat" : "0.dat");
		}

		void ModifyGenesis(
				const std::string& destination,
				uint32_t fileDatabaseBatchSize,
				const consumer<model::Block&, const model::BlockElement&>& modify) {
			// load from file storage to allow successive modifications
			io::FileBlockStorage storage(destination, fileDatabaseBatchSize);
			auto pGenesisBlockElement = storage.loadBlockElement(Height(1));

			// modify genesis block and resign it
			auto& genesisBlock = const_cast<model::Block&>(pGenesisBlockElement->Block);
			modify(genesisBlock, *pGenesisBlockElement);
			extensions::BlockExtensions(GetGenesisGenerationHashSeed()).signFullBlock(
					crypto::KeyPair::FromString(Test_Network_Genesis_Private_Key),
					genesisBlock);

			// overwrite the genesis file in destination
			// (only the block and entity hash need to be rewritten; this works because block size does not change)
			auto genesisFilename = GetGenesisFilename(destination, fileDatabaseBatchSize);
			io::RawFile genesisFile(genesisFilename, io::OpenMode::Read_Append);
			if (1 != fileDatabaseBatchSize)
				genesisFile.seek(fileDatabaseBatchSize * sizeof(uint64_t));

			genesisFile.write({ reinterpret_cast<const uint8_t*>(&genesisBlock), genesisBlock.Size });
			genesisFile.write(model::CalculateHash(genesisBlock));
		}
	}

	void ModifyGenesis(const std::string& destination, const consumer<model::Block&, const model::BlockElement&>& modify) {
		ModifyGenesis(destination, File_Database_Batch_Size, modify);
	}

	void ModifySeedGenesis(const std::string& destination, const consumer<model::Block&, const model::BlockElement&>& modify) {
		auto genesisFilename = GetGenesisFilename(destination, 1);
		std::filesystem::permissions(genesisFilename, std::filesystem::perms::owner_write, std::filesystem::perm_options::add);

		ModifyGenesis(destination, 1, modify);

		MakeReadOnly(genesisFilename);
	}

	void FakeHeight(const std::string& destination, uint64_t height) {
		const std::string genesisDirectory = "/00000";
		const std::string genesisHashFilename = destination + genesisDirectory + "/hashes.dat";

		std::vector<uint8_t> hashesBuffer(height * Hash256::Size);
		{
			io::RawFile file(genesisHashFilename, io::OpenMode::Read_Write);
			file.write(hashesBuffer);
		}

		SetIndexHeight(destination, --height);
	}
}}
