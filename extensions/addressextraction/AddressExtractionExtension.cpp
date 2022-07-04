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

#include "src/AddressExtractionBlockChangeSubscriber.h"
#include "src/AddressExtractionPtChangeSubscriber.h"
#include "src/AddressExtractionUtChangeSubscriber.h"
#include "src/AddressExtractor.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/extensions/RootedService.h"

namespace bitxorcore { namespace addressextraction {

	namespace {
		void RegisterExtension(extensions::ProcessBootstrapper& bootstrapper) {
			auto pAddressExtractor = std::make_shared<AddressExtractor>(bootstrapper.pluginManager().createNotificationPublisher());

			// add a dummy service for extending service lifetimes
			bootstrapper.extensionManager().addServiceRegistrar(extensions::CreateRootedServiceRegistrar(
					pAddressExtractor,
					"addressextraction.extractor",
					extensions::ServiceRegistrarPhase::Initial));

			// register subscriber
			auto& subscriptionManager = bootstrapper.subscriptionManager();
			subscriptionManager.addBlockChangeSubscriber(CreateAddressExtractionBlockChangeSubscriber(*pAddressExtractor));
			subscriptionManager.addUtChangeSubscriber(CreateAddressExtractionUtChangeSubscriber(*pAddressExtractor));
			subscriptionManager.addPtChangeSubscriber(CreateAddressExtractionPtChangeSubscriber(*pAddressExtractor));
		}
	}
}}

extern "C" PLUGIN_API
void RegisterExtension(bitxorcore::extensions::ProcessBootstrapper& bootstrapper) {
	bitxorcore::addressextraction::RegisterExtension(bootstrapper);
}
