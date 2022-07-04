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
#include "src/model/TokenNotifications.h"
#include "bitxorcore/model/Notifications.h"
#include "bitxorcore/observers/ObserverTypes.h"

namespace bitxorcore { namespace model { class InflationCalculator; } }

namespace bitxorcore { namespace observers {

	/// Observes changes triggered by token definition notifications and:
	/// - creates tokens
	DECLARE_OBSERVER(TokenDefinition, model::TokenDefinitionNotification)();

	/// Observes changes triggered by token supply change notifications and:
	/// - increases or decreases supply
	DECLARE_OBSERVER(TokenSupplyChange, model::TokenSupplyChangeNotification)();

	/// Observes block notifications and:
	/// - increases or decreases the supply of the currency token (\a currencyTokenId) given the inflation \a calculator
	DECLARE_OBSERVER(TokenSupplyInflation, model::BlockNotification)(
			TokenId currencyTokenId,
			const model::InflationCalculator& calculator);
}}
