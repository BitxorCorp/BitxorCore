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

#include "HashLockUtils.h"
#include "plugins/txes/lock_hash/src/model/HashLockNotifications.h"
#include "bitxorcore/model/NotificationPublisher.h"
#include "bitxorcore/model/NotificationSubscriber.h"

namespace bitxorcore { namespace unbondedpruning {

	namespace {
		class DependentTransactionCollector : public model::NotificationSubscriber {
		public:
			void notify(const model::Notification& notification) override {
				if (model::LockHash_Hash_Notification == notification.Type)
					m_hashes.insert(static_cast<const model::HashLockNotification&>(notification).Hash);
			}

		public:
			const utils::HashSet& hashes() const {
				return m_hashes;
			}

		private:
			utils::HashSet m_hashes;
		};
	}

	utils::HashSet FindDependentTransactionHashes(
			const model::TransactionInfo& transactionInfo,
			const model::NotificationPublisher& notificationPublisher) {
		model::WeakEntityInfo weakTransactionInfo(*transactionInfo.pEntity, transactionInfo.EntityHash);
		DependentTransactionCollector sub;
		notificationPublisher.publish(weakTransactionInfo, sub);
		return sub.hashes();
	}
}}
