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
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/validators/ValidatorTypes.h"
#include <memory>

namespace bitxorcore {
	namespace config { class BitxorCoreConfiguration; }
	namespace observers { class EntityObserver; }
}

namespace bitxorcore { namespace extensions {

	/// Creates plugin storage configuration from \a config.
	plugins::StorageConfiguration CreateStorageConfiguration(const config::BitxorCoreConfiguration& config);

	/// Creates a stateless entity validator using \a pluginManager that filters out notifications of \a excludedNotificationType.
	std::unique_ptr<const validators::StatelessEntityValidator> CreateStatelessEntityValidator(
			const plugins::PluginManager& manager,
			model::NotificationType excludedNotificationType = static_cast<model::NotificationType>(0));

	/// Creates an undo entity observer using \a pluginManager.
	std::unique_ptr<const observers::EntityObserver> CreateUndoEntityObserver(const plugins::PluginManager& manager);
}}
