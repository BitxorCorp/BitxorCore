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
#include "plugins/txes/lock_shared/src/model/LockNotifications.h"

namespace bitxorcore { namespace model {

	// region hash lock notification types

/// Defines a hash lock notification type with \a DESCRIPTION, \a CODE and \a CHANNEL.
#define DEFINE_HASH_LOCK_NOTIFICATION(DESCRIPTION, CODE, CHANNEL) DEFINE_NOTIFICATION_TYPE(CHANNEL, LockHash, DESCRIPTION, CODE)

	/// Hash lock duration.
	DEFINE_HASH_LOCK_NOTIFICATION(Hash_Duration, 0x0001, Validator);

	/// Hash lock token.
	DEFINE_HASH_LOCK_NOTIFICATION(Token, 0x0002, Validator);

	/// Hash lock creation.
	DEFINE_HASH_LOCK_NOTIFICATION(Hash, 0x0003, All);

#undef DEFINE_HASH_LOCK_NOTIFICATION

	// endregion

	// region HashLockTokenNotification

	/// Notification of a hash lock token.
	struct HashLockTokenNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = LockHash_Token_Notification;

	public:
		/// Creates a notification around \a token.
		explicit HashLockTokenNotification(UnresolvedToken token)
				: Notification(Notification_Type, sizeof(HashLockTokenNotification))
				, Token(token)
		{}

	public:
		/// Locked token.
		UnresolvedToken Token;
	};

	// endregion

	// region HashLockDurationNotification

	/// Notification of a hash lock duration.
	struct HashLockDurationNotification : public BaseLockDurationNotification<HashLockDurationNotification> {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = LockHash_Hash_Duration_Notification;

	public:
		using BaseLockDurationNotification<HashLockDurationNotification>::BaseLockDurationNotification;
	};

	// endregion

	// region HashLockNotification

	/// Notification of a hash lock.
	struct HashLockNotification : public BaseLockNotification<HashLockNotification> {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = LockHash_Hash_Notification;

	public:
		/// Creates hash lock notification around \a owner, \a token, \a duration and \a hash.
		HashLockNotification(const Address& owner, const UnresolvedToken& token, BlockDuration duration, const Hash256& hash)
				: BaseLockNotification(owner, token, duration)
				, Hash(hash)
		{}

	public:
		/// Hash.
		const Hash256& Hash;
	};

	// endregion
}}
