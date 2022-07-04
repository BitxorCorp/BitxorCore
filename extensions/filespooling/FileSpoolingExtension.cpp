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

#include "src/FileBlockChangeStorage.h"
#include "src/FileFinalizationStorage.h"
#include "src/FilePtChangeStorage.h"
#include "src/FileTransactionStatusStorage.h"
#include "src/FileUtChangeStorage.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/io/FileQueue.h"

namespace bitxorcore { namespace filespooling {

	namespace {
		class FileQueueFactory {
		public:
			explicit FileQueueFactory(const std::string& dataDirectory)
					: m_dataDirectory(config::BitxorCoreDataDirectoryPreparer::Prepare(dataDirectory))
			{}

		public:
			std::unique_ptr<io::FileQueueWriter> create(const std::string& queueName) const {
				return std::make_unique<io::FileQueueWriter>(m_dataDirectory.spoolDir(queueName).str());
			}

		private:
			config::BitxorCoreDataDirectory m_dataDirectory;
		};

		void RegisterExtension(extensions::ProcessBootstrapper& bootstrapper) {
			// register subscribers
			FileQueueFactory factory(bootstrapper.config().User.DataDirectory);
			auto& subscriptionManager = bootstrapper.subscriptionManager();
			subscriptionManager.addBlockChangeSubscriber(CreateFileBlockChangeStorage(factory.create("block_change")));
			subscriptionManager.addUtChangeSubscriber(CreateFileUtChangeStorage(factory.create("unconfirmed_transactions_change")));
			subscriptionManager.addPtChangeSubscriber(CreateFilePtChangeStorage(factory.create("partial_transactions_change")));
			subscriptionManager.addFinalizationSubscriber(CreateFileFinalizationStorage(factory.create("finalization")));
			subscriptionManager.addTransactionStatusSubscriber(CreateFileTransactionStatusStorage(factory.create("transaction_status")));
		}
	}
}}

extern "C" PLUGIN_API
void RegisterExtension(bitxorcore::extensions::ProcessBootstrapper& bootstrapper) {
	bitxorcore::filespooling::RegisterExtension(bootstrapper);
}
