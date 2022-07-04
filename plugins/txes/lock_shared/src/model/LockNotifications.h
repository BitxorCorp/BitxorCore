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
#include "bitxorcore/model/Token.h"
#include "bitxorcore/model/Notifications.h"

namespace bitxorcore { namespace model {

	// region BaseLockDurationNotification

	/// Base for lock duration notification.
	template<typename TDerivedNotification>
	struct BaseLockDurationNotification : public Notification {
	public:
		/// Creates a notification around \a duration.
		explicit BaseLockDurationNotification(BlockDuration duration)
				: Notification(TDerivedNotification::Notification_Type, sizeof(TDerivedNotification))
				, Duration(duration)
		{}

	public:
		/// Lock duration.
		BlockDuration Duration;
	};

	// endregion

	// region BaseLockNotification

	/// Base for lock transaction notification.
	template<typename TDerivedNotification>
	struct BaseLockNotification : public Notification {
	protected:
		/// Creates base lock notification around \a owner, \a token and \a duration.
		BaseLockNotification(const Address& owner, const UnresolvedToken& token, BlockDuration duration)
				: Notification(TDerivedNotification::Notification_Type, sizeof(TDerivedNotification))
				, Owner(owner)
				, Token(token)
				, Duration(duration)
		{}

	public:
		/// Lock owner.
		Address Owner;

		/// Locked token.
		UnresolvedToken Token;

		/// Lock duration.
		BlockDuration Duration;
	};

	// endregion
}}
