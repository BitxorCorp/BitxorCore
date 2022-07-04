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

#include "PluginLoader.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/plugins/PluginLoader.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/plugins/PluginModule.h"
#include <filesystem>

namespace bitxorcore { namespace tools { namespace plugins {

	namespace {
		// region TempDirectoryGuard

		class TempDirectoryGuard {
		public:
			explicit TempDirectoryGuard(const std::string& directoryPath) : m_directoryPath(directoryPath)
			{}

			~TempDirectoryGuard() {
				auto numRemovedFiles = std::filesystem::remove_all(m_directoryPath);
				BITXORCORE_LOG(info)
						<< "deleted directory " << m_directoryPath << " and removed " << numRemovedFiles
						<< " files (exists? " << std::filesystem::exists(m_directoryPath) << ")";
			}

		private:
			std::string m_directoryPath;
		};

		// endregion
	}

	// region PluginLoader::Impl

	class PluginLoader::Impl {
	public:
		Impl(const config::BitxorCoreConfiguration& config, CacheDatabaseCleanupMode databaseCleanupMode)
				: m_config(config)
				, m_pluginManager(m_config.Blockchain, CreateStorageConfiguration(m_config), m_config.User, m_config.Inflation) {
			// in purge mode, clean up the data directory after each execution
			// (cache database is hardcoded to "statedb" so entire data directory must be temporary)
			if (CacheDatabaseCleanupMode::Purge != databaseCleanupMode)
				return;

			if (std::filesystem::exists(m_config.User.DataDirectory))
				BITXORCORE_THROW_INVALID_ARGUMENT_1("temporary data directory must not exist", m_config.User.DataDirectory);

			auto temporaryDirectory = (std::filesystem::path(m_config.User.DataDirectory)).generic_string();
			m_pCacheDatabaseGuard = std::make_unique<TempDirectoryGuard>(temporaryDirectory);
		}

	public:
		bitxorcore::plugins::PluginManager& manager() {
			return m_pluginManager;
		}

	public:
		void loadAll() {
			// default plugins
			for (const auto& pluginName : { "bitxorcore.plugins.coresystem", "bitxorcore.plugins.signature" })
				loadPlugin(pluginName);

			// custom plugins
			for (const auto& pair : m_config.Blockchain.Plugins)
				loadPlugin(pair.first);
		}

	private:
		void loadPlugin(const std::string& pluginName) {
			LoadPluginByName(m_pluginManager, m_pluginModules, m_config.User.PluginsDirectory, pluginName);
		}

	private:
		static bitxorcore::plugins::StorageConfiguration CreateStorageConfiguration(const config::BitxorCoreConfiguration& config) {
			bitxorcore::plugins::StorageConfiguration storageConfig;
			storageConfig.PreferCacheDatabase = config.Node.EnableCacheDatabaseStorage;
			storageConfig.CacheDatabaseDirectory = (std::filesystem::path(config.User.DataDirectory) / "statedb").generic_string();
			return storageConfig;
		}

	private:
		const config::BitxorCoreConfiguration& m_config;
		std::vector<bitxorcore::plugins::PluginModule> m_pluginModules;
		bitxorcore::plugins::PluginManager m_pluginManager;

		std::unique_ptr<TempDirectoryGuard> m_pCacheDatabaseGuard;
	};

	// endregion

	// region PluginLoader

	PluginLoader::PluginLoader(const config::BitxorCoreConfiguration& config, CacheDatabaseCleanupMode databaseCleanupMode)
			: m_pImpl(std::make_unique<Impl>(config, databaseCleanupMode))
	{}

	PluginLoader::~PluginLoader() = default;

	bitxorcore::plugins::PluginManager& PluginLoader::manager() {
		return m_pImpl->manager();
	}

	const model::TransactionRegistry& PluginLoader::transactionRegistry() const {
		return m_pImpl->manager().transactionRegistry();
	}

	std::unique_ptr<const model::NotificationPublisher> PluginLoader::createNotificationPublisher() const {
		return m_pImpl->manager().createNotificationPublisher();
	}

	void PluginLoader::loadAll() {
		m_pImpl->loadAll();
	}

	// endregion
}}}
