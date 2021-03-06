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
#include "ObserverTypes.h"
#include "bitxorcore/model/NotificationPublisher.h"

namespace bitxorcore { namespace model { class TransactionRegistry; } }

namespace bitxorcore { namespace observers {

	/// Notification observer to entity observer adapter that reverses the order of raised notifications.
	class ReverseNotificationObserverAdapter : public EntityObserver {
	private:
		using NotificationObserverPointer = NotificationObserverPointerT<model::Notification>;
		using NotificationPublisherPointer = std::unique_ptr<const model::NotificationPublisher>;

	public:
		/// Creates a new adapter around \a pObserver and \a pPublisher.
		ReverseNotificationObserverAdapter(NotificationObserverPointer&& pObserver, NotificationPublisherPointer&& pPublisher);

	public:
		const std::string& name() const override;

		void notify(const model::WeakEntityInfo& entityInfo, ObserverContext& context) const override;

	private:
		NotificationObserverPointer m_pObserver;
		NotificationPublisherPointer m_pPublisher;
	};
}}
