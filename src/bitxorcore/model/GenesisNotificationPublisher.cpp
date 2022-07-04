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

#include "GenesisNotificationPublisher.h"
#include "BlockchainConfiguration.h"
#include "NotificationSubscriber.h"

namespace bitxorcore { namespace model {

	GenesisNotificationPublisherOptions ExtractGenesisNotificationPublisherOptions(const BlockchainConfiguration& config) {
		GenesisNotificationPublisherOptions options;

		if (0 < config.HarvestNetworkPercentage)
			options.SpecialAccountAddresses.insert(GetHarvestNetworkFeeSinkAddress(config).get(Height(1)));

		return options;
	}

	namespace {
		class GenesisNotificationPublisherDecorator : public NotificationPublisher {
		public:
			GenesisNotificationPublisherDecorator(
					std::unique_ptr<const NotificationPublisher>&& pPublisher,
					const GenesisNotificationPublisherOptions& options)
					: m_pPublisher(std::move(pPublisher))
					, m_options(options)
			{}

		public:
			void publish(const WeakEntityInfo& entityInfo, NotificationSubscriber& sub) const override {
				for (const auto& address : m_options.SpecialAccountAddresses)
					sub.notify(AccountAddressNotification(address));

				m_pPublisher->publish(entityInfo, sub);
			}

		private:
			std::unique_ptr<const NotificationPublisher> m_pPublisher;
			const GenesisNotificationPublisherOptions& m_options;
		};
	}

	std::unique_ptr<const NotificationPublisher> CreateGenesisNotificationPublisher(
			std::unique_ptr<const NotificationPublisher>&& pPublisher,
			const GenesisNotificationPublisherOptions& options) {
		return std::make_unique<GenesisNotificationPublisherDecorator>(std::move(pPublisher), options);
	}
}}
