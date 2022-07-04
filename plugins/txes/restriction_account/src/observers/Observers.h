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
#include "src/model/AccountRestrictionNotifications.h"
#include "bitxorcore/observers/ObserverTypes.h"

namespace bitxorcore { namespace observers {

	/// Observes changes triggered by account address restriction value modification notifications and:
	/// - adds / deletes account address restriction value to / from cache
	DECLARE_OBSERVER(AccountAddressRestrictionValueModification, model::ModifyAccountAddressRestrictionValueNotification)();

	/// Observes changes triggered by account token restriction value modification notifications and:
	/// - adds / deletes account token restriction value to / from cache
	DECLARE_OBSERVER(AccountTokenRestrictionValueModification, model::ModifyAccountTokenRestrictionValueNotification)();

	/// Observes changes triggered by account operation restriction value modification notifications and:
	/// - adds / deletes account operation restriction value to / from cache
	DECLARE_OBSERVER(AccountOperationRestrictionValueModification, model::ModifyAccountOperationRestrictionValueNotification)();
}}
