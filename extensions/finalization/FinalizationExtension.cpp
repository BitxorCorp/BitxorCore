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

#include "finalization/src/io/FileProofStorage.h"
#include "src/FinalizationBootstrapperService.h"
#include "src/FinalizationConfiguration.h"
#include "src/FinalizationMessageProcessingService.h"
#include "src/FinalizationOrchestratorService.h"
#include "src/FinalizationService.h"
#include "src/FinalizationSyncSourceService.h"
#include "bitxorcore/config/ConfigurationFileLoader.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"

namespace bitxorcore { namespace finalization {

	namespace {
		void RegisterExtension(extensions::ProcessBootstrapper& bootstrapper) {
			const auto& resourcesPath = bootstrapper.resourcesPath();
			auto config = FinalizationConfiguration::LoadFromPath(resourcesPath);
			config.VotingSetGrouping = bootstrapper.config().Blockchain.VotingSetGrouping;

			auto pProofStorage = std::make_unique<io::FileProofStorage>(
					bootstrapper.config().User.DataDirectory,
					bootstrapper.config().Node.FileDatabaseBatchSize);

			// register other services
			auto& extensionManager = bootstrapper.extensionManager();
			extensionManager.addServiceRegistrar(CreateFinalizationBootstrapperServiceRegistrar(config, std::move(pProofStorage)));
			extensionManager.addServiceRegistrar(CreateFinalizationBootstrapperPhaseTwoServiceRegistrar());
			extensionManager.addServiceRegistrar(CreateFinalizationServiceRegistrar(config));
			extensionManager.addServiceRegistrar(CreateFinalizationSyncSourceServiceRegistrar(config.EnableVoting));

			if (config.EnableVoting) {
				extensionManager.addServiceRegistrar(CreateFinalizationMessageProcessingServiceRegistrar(config));
				extensionManager.addServiceRegistrar(CreateFinalizationOrchestratorServiceRegistrar(config));
			}
		}
	}
}}

extern "C" PLUGIN_API
void RegisterExtension(bitxorcore::extensions::ProcessBootstrapper& bootstrapper) {
	bitxorcore::finalization::RegisterExtension(bootstrapper);
}
