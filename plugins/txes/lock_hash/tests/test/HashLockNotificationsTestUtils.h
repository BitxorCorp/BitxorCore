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
#include "HashLockInfoCacheTestUtils.h"
#include "src/model/HashLockNotifications.h"
#include "tests/test/nodeps/Random.h"

namespace bitxorcore { namespace test {

	/// Hash lock notification builder.
	struct HashLockNotificationBuilder {
	public:
		/// Creates hash lock notification builder.
		HashLockNotificationBuilder() {
			FillWithRandomData({ reinterpret_cast<uint8_t*>(this), sizeof(HashLockNotificationBuilder) });
		}

		/// Creates a notification.
		auto notification() {
			return model::HashLockNotification(m_owner, m_token, m_duration, m_hash);
		}

		/// Prepares the builder using \a lockInfo.
		void prepare(const state::HashLockInfo& lockInfo) {
			m_hash = lockInfo.Hash;
		}

	private:
		Address m_owner;
		model::UnresolvedToken m_token;
		BlockDuration m_duration;
		Hash256 m_hash;
	};
}}
