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
#include "bitxorcore/model/NetworkIdentifier.h"
#include "bitxorcore/model/NotificationPublisher.h"
#include "bitxorcore/observers/ObserverTypes.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace chain {

	/// Configuration for creating contexts for executing entities.
	struct ExecutionContextConfiguration {
	private:
		using ResolverContextFactoryFunc = std::function<model::ResolverContext (const cache::ReadOnlyBitxorCoreCache&)>;

	public:
		/// Network info.
		model::NetworkInfo Network;

		/// Resolver context factory.
		ResolverContextFactoryFunc ResolverContextFactory;
	};

	/// Configuration for executing entities.
	struct ExecutionConfiguration : public ExecutionContextConfiguration {
	private:
		using ObserverPointer = std::shared_ptr<const observers::AggregateNotificationObserver>;
		using ValidatorPointer = std::shared_ptr<const validators::stateful::AggregateNotificationValidator>;
		using PublisherPointer = std::shared_ptr<const model::NotificationPublisher>;

	public:

		/// Observer.
		ObserverPointer pObserver;

		/// Stateful validator.
		ValidatorPointer pValidator;

		/// Notification publisher.
		PublisherPointer pNotificationPublisher;
	};
}}
