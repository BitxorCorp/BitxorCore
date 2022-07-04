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

#include "src/MessagingConfiguration.h"
#include "src/ZeroMqBlockChangeSubscriber.h"
#include "src/ZeroMqEntityPublisher.h"
#include "src/ZeroMqFinalizationSubscriber.h"
#include "src/ZeroMqPtChangeSubscriber.h"
#include "src/ZeroMqTransactionStatusSubscriber.h"
#include "src/ZeroMqUtChangeSubscriber.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/extensions/RootedService.h"
#include "bitxorcore/model/NotificationPublisher.h"

namespace bitxorcore { namespace zeromq {

	namespace {
		void RegisterExtension(extensions::ProcessBootstrapper& bootstrapper) {
			auto config = MessagingConfiguration::LoadFromPath(bootstrapper.resourcesPath());
			auto pZeroEntityPublisher = std::make_shared<ZeroMqEntityPublisher>(
					config.ListenInterface,
					config.SubscriberPort,
					bootstrapper.pluginManager().createNotificationPublisher());

			// add a dummy service for extending service lifetimes
			bootstrapper.extensionManager().addServiceRegistrar(extensions::CreateRootedServiceRegistrar(
					pZeroEntityPublisher,
					"zeromq.publisher",
					extensions::ServiceRegistrarPhase::Initial));

			// register subscriptions
			auto& subscriptionManager = bootstrapper.subscriptionManager();
			subscriptionManager.addBlockChangeSubscriber(CreateZeroMqBlockChangeSubscriber(*pZeroEntityPublisher));
			subscriptionManager.addPtChangeSubscriber(CreateZeroMqPtChangeSubscriber(*pZeroEntityPublisher));
			subscriptionManager.addUtChangeSubscriber(CreateZeroMqUtChangeSubscriber(*pZeroEntityPublisher));
			subscriptionManager.addFinalizationSubscriber(CreateZeroMqFinalizationSubscriber(*pZeroEntityPublisher));
			subscriptionManager.addTransactionStatusSubscriber(CreateZeroMqTransactionStatusSubscriber(*pZeroEntityPublisher));
		}
	}
}}

extern "C" PLUGIN_API
void RegisterExtension(bitxorcore::extensions::ProcessBootstrapper& bootstrapper) {
	bitxorcore::zeromq::RegisterExtension(bootstrapper);
}
