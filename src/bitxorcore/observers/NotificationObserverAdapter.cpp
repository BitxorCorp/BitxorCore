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

#include "NotificationObserverAdapter.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPlugin.h"

namespace bitxorcore { namespace observers {

	namespace {
		class ObservingNotificationSubscriber : public model::NotificationSubscriber {
		public:
			ObservingNotificationSubscriber(const NotificationObserver& observer, ObserverContext& context)
					: m_observer(observer)
					, m_context(context)
			{}

		public:
			void notify(const model::Notification& notification) override {
				if (!IsSet(notification.Type, model::NotificationChannel::Observer))
					return;

				m_observer.notify(notification, m_context);
			}

		private:
			const NotificationObserver& m_observer;
			ObserverContext& m_context;
		};
	}

	NotificationObserverAdapter::NotificationObserverAdapter(
			NotificationObserverPointer&& pObserver,
			NotificationPublisherPointer&& pPublisher)
			: m_pObserver(std::move(pObserver))
			, m_pPublisher(std::move(pPublisher))
	{}

	const std::string& NotificationObserverAdapter::name() const {
		return m_pObserver->name();
	}

	void NotificationObserverAdapter::notify(const model::WeakEntityInfo& entityInfo, ObserverContext& context) const {
		ObservingNotificationSubscriber sub(*m_pObserver, context);
		m_pPublisher->publish(entityInfo, sub);
	}
}}
