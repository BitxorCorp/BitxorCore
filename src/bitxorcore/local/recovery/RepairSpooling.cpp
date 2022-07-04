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

#include "RepairSpooling.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/io/FilesystemUtils.h"
#include "bitxorcore/io/IndexFile.h"

namespace bitxorcore { namespace local {

	namespace {
		class SpoolingRepairer {
		public:
			explicit SpoolingRepairer(const config::BitxorCoreDataDirectory& dataDirectory) : m_dataDirectory(dataDirectory)
			{}

		public:
			void retain(const std::string& queueName) {
				BITXORCORE_LOG(debug) << " - leaving " << queueName << " unchanged";
				// no recovery actions are needed
			}

			void purge(const std::string& queueName) {
				BITXORCORE_LOG(debug) << " - purging " << queueName;
				io::PurgeDirectory(m_dataDirectory.spoolDir(queueName).str());
			}

			void reindex(
					const std::string& queueName,
					const std::string& destinationIndexFilename,
					const std::string& sourceIndexFilename) {
				auto directory = m_dataDirectory.spoolDir(queueName);
				io::IndexFile destinationIndexFile(directory.file(destinationIndexFilename));
				io::IndexFile sourceIndexFile(directory.file(sourceIndexFilename));

				if (!sourceIndexFile.exists()) {
					BITXORCORE_LOG(debug) << " - index " << sourceIndexFilename << " does not exist, skipping";
					return;
				}

				BITXORCORE_LOG(debug) << " - setting " << destinationIndexFilename << " to " << sourceIndexFilename;
				destinationIndexFile.set(sourceIndexFile.get());
			}

		private:
			config::BitxorCoreDataDirectory m_dataDirectory;
		};
	}

	void RepairSpooling(const config::BitxorCoreDataDirectory& dataDirectory, consumers::CommitOperationStep commitStep) {
		SpoolingRepairer repairer(dataDirectory);

		// broker process should be able to process these queues as-is on next reboot
		// - block_change messages are produced after State_Written, so always safe to process
		// - finalization messages are independent of state change
		// - transaction_status messages are permanent and have no impact on state
		repairer.retain("block_change");
		repairer.retain("finalization");
		repairer.retain("transaction_status");

		// recovery assumes that both broker and server processes are stopped
		// as a result, these queues can safely be deleted because they contain only transient data
		repairer.purge("partial_transactions_change");
		repairer.purge("unconfirmed_transactions_change");

		// remove temporary directory used by recovery orchestrator during chain load
		repairer.purge("block_recover");

		if (consumers::CommitOperationStep::State_Written == commitStep)
			return;

		// block_sync data can never be applied because corresponding state has not been completely written
		repairer.purge("block_sync");

		// state_change data has not been completely written
		// remember the three index files in place:
		// - index_server.dat               | incremented by StateChange handler in commitAll
		// - >= index.dat                   | incremented by CommitStep(All_Updated) handler in commitAll
		// - >= index_(server|broker)_r.dat | incremented by cleanup consumer or broker consumption
		// data in range (index.dat, index_server.dat] is safe to ignore
		repairer.reindex("state_change", "index_server.dat", "index.dat");
	}
}}
