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
#include "bitxorcore/model/NotificationPublisher.h"
#include "bitxorcore/model/TransactionPlugin.h"
#include <memory>

namespace bitxorcore {
	namespace config { class BitxorCoreConfiguration; }
	namespace plugins { class PluginManager; }
}

namespace bitxorcore { namespace tools { namespace plugins {

	/// Possible cache database cleanup modes.
	enum class CacheDatabaseCleanupMode {
		/// Perform no cleanup.
		None,

		/// Purge after execution.
		Purge
	};

	/// Loads plugins into a plugin manager.
	class PluginLoader {
	public:
		/// Creates a loader around \a config with specified database cleanup mode (\a databaseCleanupMode).
		PluginLoader(const config::BitxorCoreConfiguration& config, CacheDatabaseCleanupMode databaseCleanupMode);

		/// Destroys the loader.
		~PluginLoader();

	public:
		/// Gets the plugin manager.
		bitxorcore::plugins::PluginManager& manager();

		/// Gets the transaction registry.
		const model::TransactionRegistry& transactionRegistry() const;

		/// Creates a notification publisher.
		std::unique_ptr<const model::NotificationPublisher> createNotificationPublisher() const;

	public:
		/// Loads all configured plugins.
		void loadAll();

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}}}
