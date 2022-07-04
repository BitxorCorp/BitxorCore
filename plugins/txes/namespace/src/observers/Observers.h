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
#include "src/model/AliasNotifications.h"
#include "src/model/NamespaceNotifications.h"
#include "bitxorcore/model/Notifications.h"
#include "bitxorcore/observers/ObserverTypes.h"

namespace bitxorcore { namespace observers {

	// region alias

	/// Observes changes triggered by aliased address notifications and:
	/// - links/unlinks address to namespace
	DECLARE_OBSERVER(AliasedAddress, model::AliasedAddressNotification)();

	/// Observes changes triggered by aliased token id notifications and:
	/// - links/unlinks token id to namespace
	DECLARE_OBSERVER(AliasedTokenId, model::AliasedTokenIdNotification)();

	// endregion

	// region namespace

	/// Observes changes triggered by root namespace notifications and:
	/// - creates (root) namespaces
	DECLARE_OBSERVER(RootNamespace, model::RootNamespaceNotification)();

	/// Observes changes triggered by child namespace notifications and:
	/// - creates (child) namespaces
	DECLARE_OBSERVER(ChildNamespace, model::ChildNamespaceNotification)();

	/// Observes changes triggered by iso code namespace notifications and:
	/// - creates (iso) namespaces
	DECLARE_OBSERVER(IsoCodeNamespace, model::IsoCodeNamespaceNotification)();

	// endregion
}}
