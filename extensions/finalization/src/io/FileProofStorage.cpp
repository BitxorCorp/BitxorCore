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

#include "FileProofStorage.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/BufferedFileStream.h"
#include "bitxorcore/io/PodIoUtils.h"

namespace bitxorcore { namespace io {

	// region FinalizationIndexFile

	FileProofStorage::FinalizationIndexFile::FinalizationIndexFile(const std::string& filename, LockMode lockMode)
			: m_filename(filename)
			, m_lockMode(lockMode)
	{}

	bool FileProofStorage::FinalizationIndexFile::exists() const {
		return std::filesystem::is_regular_file(m_filename);
	}

	model::FinalizationStatistics FileProofStorage::FinalizationIndexFile::get() const {
		auto indexFile = open(OpenMode::Read_Only);
		model::FinalizationStatistics finalizationStatistics;
		if (sizeof(model::FinalizationStatistics) != indexFile.size())
			return model::FinalizationStatistics();

		indexFile.read({ reinterpret_cast<uint8_t*>(&finalizationStatistics), sizeof(model::FinalizationStatistics) });
		return finalizationStatistics;
	}

	void FileProofStorage::FinalizationIndexFile::set(const model::FinalizationStatistics& finalizationStatistics) {
		auto indexFile = open(OpenMode::Read_Append);
		indexFile.seek(0);
		indexFile.write({ reinterpret_cast<const uint8_t*>(&finalizationStatistics), sizeof(model::FinalizationStatistics) });
	}

	RawFile FileProofStorage::FinalizationIndexFile::open(OpenMode mode) const {
		return RawFile(m_filename, mode, m_lockMode);
	}

	// endregion

	// region FileProofStorage

	FileProofStorage::FileProofStorage(const std::string& dataDirectory, uint32_t fileDatabaseBatchSize)
			: m_database(config::BitxorCoreDirectory(dataDirectory), { fileDatabaseBatchSize, ".proof" })
			, m_indexFile((std::filesystem::path(dataDirectory) / "proof.index.dat").generic_string())
	{}

	model::FinalizationStatistics FileProofStorage::statistics() const {
		if (!m_indexFile.exists())
			return model::FinalizationStatistics();

		return m_indexFile.get();
	}

	namespace {
		std::shared_ptr<model::FinalizationProof> ReadFinalizationProof(InputStream& proofStream) {
			auto size = Read32(proofStream);
			auto pProof = utils::MakeSharedWithSize<model::FinalizationProof>(size);
			pProof->Size = size;
			proofStream.read({ reinterpret_cast<uint8_t*>(pProof.get()) + sizeof(uint32_t), size - sizeof(uint32_t) });
			return pProof;
		}
	}

	std::shared_ptr<const model::FinalizationProof> FileProofStorage::loadProof(FinalizationEpoch epoch) const {
		if (FinalizationEpoch() == epoch)
			BITXORCORE_THROW_INVALID_ARGUMENT("loadProof called with epoch 0");

		auto currentEpoch = statistics().Round.Epoch;
		if (currentEpoch < epoch) {
			std::ostringstream out;
			out << "cannot load proof with epoch " << epoch << " when storage epoch is " << currentEpoch;
			BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
		}

		auto pProofStream = m_database.inputStream(epoch.unwrap());
		return ReadFinalizationProof(*pProofStream);
	}

	std::shared_ptr<const model::FinalizationProof> FileProofStorage::loadProof(Height height) const {
		if (Height() == height)
			BITXORCORE_THROW_INVALID_ARGUMENT("loadProof called with height 0");

		auto currentHeight = statistics().Height;
		if (currentHeight < height) {
			std::ostringstream out;
			out << "cannot load proof with height " << height << " when storage height is " << currentHeight;
			BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
		}

		auto pProof = loadClosestProof(height);
		if (!pProof || pProof->Height != height) {
			BITXORCORE_LOG(debug) << "proof not found at height " << height;
			return nullptr;
		}

		return pProof;
	}

	void FileProofStorage::saveProof(const model::FinalizationProof& proof) {
		auto currentStatistics = statistics();
		if (currentStatistics.Round > proof.Round || proof.Round.Epoch > currentStatistics.Round.Epoch + FinalizationEpoch(1)) {
			std::ostringstream out;
			out << "cannot save proof with round " << proof.Round << " when storage round is " << currentStatistics.Round;
			BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
		}

		if (currentStatistics.Height > proof.Height) {
			std::ostringstream out;
			out << "cannot save proof with height " << proof.Height << " when storage height is " << currentStatistics.Height;
			BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
		}

		{
			auto pProofStream = m_database.outputStream(proof.Round.Epoch.unwrap());
			pProofStream->write({ reinterpret_cast<const uint8_t*>(&proof), proof.Size });
			pProofStream->flush();
		}

		m_indexFile.set({ proof.Round, proof.Height, proof.Hash });
	}

	std::shared_ptr<const model::FinalizationProof> FileProofStorage::loadClosestProof(Height height) const {
		auto currentEpoch = statistics().Round.Epoch;
		if (currentEpoch < FinalizationEpoch(2))
			return loadProof(FinalizationEpoch(1));

		// when the second epoch is fully finalized (and a third epoch has been started), the height of the second epoch is
		// equal to the voting set grouping
		auto pProof = loadProof(FinalizationEpoch(2));

		auto votingSetGrouping = pProof->Height.unwrap();
		auto epoch = FinalizationEpoch(static_cast<uint32_t>(height.unwrap() / votingSetGrouping + 1));
		return loadProof(epoch);
	}

	// endregion
}}
