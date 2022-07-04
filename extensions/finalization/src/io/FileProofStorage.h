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
#include "ProofStorage.h"
#include "bitxorcore/io/FileDatabase.h"
#include "bitxorcore/io/FixedSizeValueStorage.h"
#include "bitxorcore/io/IndexFile.h"
#include <string>

namespace bitxorcore { namespace io {

	/// File-based proof storage.
	class FileProofStorage final : public ProofStorage {
	public:
		/// Creates a file-based proof storage, where proofs will be stored inside \a dataDirectory
		/// with a file database batch size of \a fileDatabaseBatchSize
		FileProofStorage(const std::string& dataDirectory, uint32_t fileDatabaseBatchSize);

	public:
		model::FinalizationStatistics statistics() const override;
		std::shared_ptr<const model::FinalizationProof> loadProof(FinalizationEpoch epoch) const override;
		std::shared_ptr<const model::FinalizationProof> loadProof(Height height) const override;
		void saveProof(const model::FinalizationProof& proof) override;

	private:
		std::shared_ptr<const model::FinalizationProof> loadClosestProof(Height height) const;

	private:
		class FinalizationIndexFile {
		public:
			explicit FinalizationIndexFile(const std::string& filename, LockMode lockMode = LockMode::File);

		public:
			bool exists() const;

			model::FinalizationStatistics get() const;

		public:
			void set(const model::FinalizationStatistics& finalizationStatistics);

		private:
			RawFile open(OpenMode mode) const;

		private:
			std::string m_filename;
			LockMode m_lockMode;
		};

	private:
		FileDatabase m_database;
		FinalizationIndexFile m_indexFile;
	};
}}
