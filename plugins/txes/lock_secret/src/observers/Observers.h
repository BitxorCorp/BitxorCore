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
#include "src/model/SecretLockNotifications.h"
#include "bitxorcore/model/Notifications.h"
#include "bitxorcore/observers/ObserverTypes.h"

namespace bitxorcore { namespace observers {

	/// Observes changes triggered by secret lock notifications and:
	/// - adds/removes secret lock info to/from secret lock info cache
	/// - debits/credits lock owner
	DECLARE_OBSERVER(SecretLock, model::SecretLockNotification)();

	/// Observes changes triggered by proof notifications and:
	/// - credits/debits proof publisher
	/// - marks proper secret lock as used/unused
	DECLARE_OBSERVER(Proof, model::ProofPublicationNotification)();

	/// Observes block notifications and:
	/// - handles expired secret lock infos
	/// - credits the lock creator the tokens given in the lock info
	DECLARE_OBSERVER(ExpiredSecretLockInfo, model::BlockNotification)();
}}
