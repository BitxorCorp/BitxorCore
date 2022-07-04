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

#include "RepairState.h"
#include "bitxorcore/cache/CacheChangesStorage.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/subscribers/BrokerMessageReaders.h"
#include "bitxorcore/subscribers/StateChangeReader.h"
#include "bitxorcore/subscribers/StateChangeSubscriber.h"

namespace bitxorcore { namespace local {

	namespace {
		// region StateRepairer

		class StateRepairer {
		public:
			StateRepairer(const config::BitxorCoreDirectory& stateChangeDirectory, const cache::BitxorCoreCache& bitxorcoreCache)
					: m_stateChangeDirectory(stateChangeDirectory)
					, m_bitxorcoreCache(bitxorcoreCache)
					, m_isBrokerRecovery(std::filesystem::exists(m_stateChangeDirectory.file("index_broker_r.dat")))
			{}

		public:
			bool isBrokerRecovery() const {
				return m_isBrokerRecovery;
			}

		public:
			void readAll(
					const std::string& indexReaderFilename,
					const std::string& indexWriterFilename,
					subscribers::StateChangeSubscriber& stateChangeSubscriber) {
				subscribers::ReadAll(
						{ m_stateChangeDirectory.str(), indexReaderFilename, indexWriterFilename },
						stateChangeSubscriber,
						[&bitxorcoreCache = m_bitxorcoreCache](auto& inputStream, auto& subscriber) {
							return subscribers::ReadNextStateChange(inputStream, bitxorcoreCache.changesStorages(), subscriber);
						});
			}

			void reindex(const std::string& destinationIndexFilename, const std::string& sourceIndexFilename) {
				io::IndexFile destinationIndexFile(m_stateChangeDirectory.file(destinationIndexFilename));
				io::IndexFile sourceIndexFile(m_stateChangeDirectory.file(sourceIndexFilename));
				destinationIndexFile.set(sourceIndexFile.get());
			}

		private:
			config::BitxorCoreDirectory m_stateChangeDirectory;
			const cache::BitxorCoreCache& m_bitxorcoreCache;
			bool m_isBrokerRecovery;
		};

		// endregion
	}

	void RepairState(
			const config::BitxorCoreDirectory& stateChangeDirectory,
			const cache::BitxorCoreCache& bitxorcoreCache,
			subscribers::StateChangeSubscriber& registeredSubscriber,
			subscribers::StateChangeSubscriber& repairSubscriber) {
		StateRepairer repairer(stateChangeDirectory, bitxorcoreCache);
		auto finalIndexReaderFilename = repairer.isBrokerRecovery() ? "index_broker_r.dat" : "index_server_r.dat";

		// 1. catch up registered subscribers
		repairer.readAll(finalIndexReaderFilename, "index.dat", registeredSubscriber);

		// 2. repair and forward to registered subscribers
		repairer.readAll("index.dat", "index_server.dat", repairSubscriber);

		// 3. fixup final reader index
		repairer.reindex(finalIndexReaderFilename, "index.dat");
	}
}}
