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

#include "PluginUtils.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "bitxorcore/observers/ReverseNotificationObserverAdapter.h"
#include "bitxorcore/validators/NotificationValidatorAdapter.h"
#include "bitxorcore/preprocessor.h"

namespace bitxorcore { namespace extensions {

	plugins::StorageConfiguration CreateStorageConfiguration(const config::BitxorCoreConfiguration& config) {
		plugins::StorageConfiguration storageConfig;
		storageConfig.PreferCacheDatabase = config.Node.EnableCacheDatabaseStorage;
		storageConfig.CacheDatabaseDirectory = (std::filesystem::path(config.User.DataDirectory) / "statedb").generic_string();
		storageConfig.CacheDatabaseConfig = config.Node.CacheDatabase;
		return storageConfig;
	}

	namespace {
		template<typename TAdapter, typename TAdaptee>
		auto MakeAdapter(const plugins::PluginManager& manager, std::unique_ptr<TAdaptee>&& pAdaptee) {
			return std::make_unique<TAdapter>(std::move(pAdaptee), manager.createNotificationPublisher());
		}
	}

	std::unique_ptr<const validators::StatelessEntityValidator> CreateStatelessEntityValidator(
			const plugins::PluginManager& manager,
			model::NotificationType excludedNotificationType) {
		auto pAdapter = MakeAdapter<validators::NotificationValidatorAdapter>(manager, manager.createStatelessValidator());
		pAdapter->setExclusionFilter([excludedNotificationType](auto notificationType) {
			return excludedNotificationType == notificationType;
		});
		return PORTABLE_MOVE(pAdapter);
	}

	std::unique_ptr<const observers::EntityObserver> CreateUndoEntityObserver(const plugins::PluginManager& manager) {
		return MakeAdapter<observers::ReverseNotificationObserverAdapter>(manager, manager.createObserver());
	}
}}
