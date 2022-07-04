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
#include "ExtensionManager.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/plugins/PluginModule.h"
#include "bitxorcore/subscribers/SubscriptionManager.h"
#include "bitxorcore/thread/MultiServicePool.h"
#include "bitxorcore/plugins.h"

namespace bitxorcore { namespace extensions {

	/// Possible process dispositions.
	enum class ProcessDisposition {
		/// Production process.
		Production,

		/// Recovery process.
		Recovery
	};

	/// Process bootstrapper.
	class PLUGIN_API_DEPENDENCY ProcessBootstrapper {
	public:
		/// Creates a process bootstrapper around \a config, \a resourcesPath, \a disposition and \a servicePoolName.
		ProcessBootstrapper(
				const config::BitxorCoreConfiguration& config,
				const std::string& resourcesPath,
				ProcessDisposition disposition,
				const std::string& servicePoolName);

	public:
		/// Gets the configuration.
		const config::BitxorCoreConfiguration& config() const;

		/// Gets the resources path.
		const std::string& resourcesPath() const;

		/// Gets the process disposition.
		ProcessDisposition disposition() const;

		/// Gets the static (pretrusted) nodes.
		const std::vector<ionet::Node>& staticNodes() const;

	public:
		/// Gets the multiservice pool.
		thread::MultiServicePool& pool();

		/// Gets the extension manager.
		ExtensionManager& extensionManager();

		/// Gets the subscription manager.
		subscribers::SubscriptionManager& subscriptionManager();

		/// Gets the plugin manager.
		plugins::PluginManager& pluginManager();

	public:
		/// Loads all configured extensions.
		void loadExtensions();

		/// Adds static \a nodes to the bootstrapper.
		void addStaticNodes(const std::vector<ionet::Node>& nodes);

	private:
		config::BitxorCoreConfiguration m_config;
		std::string m_resourcesPath;
		ProcessDisposition m_disposition;
		std::unique_ptr<thread::MultiServicePool> m_pMultiServicePool;
		std::vector<plugins::PluginModule> m_extensionModules;

		ExtensionManager m_extensionManager;
		subscribers::SubscriptionManager m_subscriptionManager;
		plugins::PluginManager m_pluginManager;
		std::vector<ionet::Node> m_nodes;
	};

	/// Adds static nodes from \a path to \a bootstrapper.
	void AddStaticNodesFromPath(ProcessBootstrapper& bootstrapper, const std::string& path);
}}

/// Entry point for registering a dynamic extension module with \a bootstrapper.
extern "C" PLUGIN_API
void RegisterExtension(bitxorcore::extensions::ProcessBootstrapper& bootstrapper);
