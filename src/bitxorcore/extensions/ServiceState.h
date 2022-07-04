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
#include "ServerHooks.h"
#include "ServiceState.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/ionet/NodeInfo.h"
#include "bitxorcore/ionet/PacketHandlers.h"
#include "bitxorcore/net/PacketIoPickerContainer.h"
#include "bitxorcore/thread/Task.h"

namespace bitxorcore {
	namespace cache {
		class BitxorCoreCache;
		class ReadWriteUtCache;
		class UtCache;
	}
	namespace extensions {
		class LocalNodeChainScore;
		struct SelectorSettings;
	}
	namespace io { class BlockStorageCache; }
	namespace ionet { class NodeContainer; }
	namespace plugins { class PluginManager; }
	namespace subscribers {
		class FinalizationSubscriber;
		class NodeSubscriber;
		class StateChangeSubscriber;
		class TransactionStatusSubscriber;
	}
	namespace thread { class MultiServicePool; }
	namespace utils { class DiagnosticCounter; }
}

namespace bitxorcore { namespace extensions {

	// region ServiceState

	/// State that is used as part of service registration.
	class ServiceState {
	public:
		/// Creates service state around \a config, \a nodes, \a cache, \a storage, \a score, \a utCache, \a timeSupplier
		/// \a finalizationSubscriber, \a nodeSubscriber, \a stateChangeSubscriber, \a transactionStatusSubscriber,
		/// \a counters, \a pluginManager and \a pool.
		ServiceState(
				const config::BitxorCoreConfiguration& config,
				ionet::NodeContainer& nodes,
				cache::BitxorCoreCache& cache,
				io::BlockStorageCache& storage,
				LocalNodeChainScore& score,
				cache::MemoryUtCacheProxy& utCache,
				const supplier<Timestamp>& timeSupplier,
				subscribers::FinalizationSubscriber& finalizationSubscriber,
				subscribers::NodeSubscriber& nodeSubscriber,
				subscribers::StateChangeSubscriber& stateChangeSubscriber,
				subscribers::TransactionStatusSubscriber& transactionStatusSubscriber,
				const std::vector<utils::DiagnosticCounter>& counters,
				const plugins::PluginManager& pluginManager,
				thread::MultiServicePool& pool)
				: m_config(config)
				, m_nodes(nodes)
				, m_cache(cache)
				, m_storage(storage)
				, m_score(score)
				/**
				*** cannot hold reference to MemoryUtCacheProxy because it is not allowed to be called across modules
				*** (it does not have type_visibility attribute) and ServiceState is passed to extensions.
				*** instead, call MemoryUtCacheProxy::get (both return types with type_visibility attribute) during ServiceState
				*** construction, which is expected to be in same module as MemoryUtCacheProxy.
				**/
				, m_readWriteUtCache(const_cast<const cache::MemoryUtCacheProxy&>(utCache).get())
				, m_utCache(utCache.get())
				, m_timeSupplier(timeSupplier)
				, m_finalizationSubscriber(finalizationSubscriber)
				, m_nodeSubscriber(nodeSubscriber)
				, m_stateChangeSubscriber(stateChangeSubscriber)
				, m_transactionStatusSubscriber(transactionStatusSubscriber)
				, m_counters(counters)
				, m_pluginManager(pluginManager)
				, m_pool(pool)
				, m_packetHandlers(m_config.Node.MaxPacketDataSize.bytes32())
		{}

	public:
		/// Gets the config.
		const auto& config() const {
			return m_config;
		}

		/// Gets the nodes.
		auto& nodes() const {
			return m_nodes;
		}

		/// Gets the cache.
		auto& cache() const {
			return m_cache;
		}

		/// Gets the storage.
		auto& storage() const {
			return m_storage;
		}

		/// Gets the score.
		auto& score() const {
			return m_score;
		}

		/// Gets the unconfirmed transactions cache.
		const cache::ReadWriteUtCache& utCache() const {
			return m_readWriteUtCache;
		}

		/// Gets the unconfirmed transactions cache.
		cache::UtCache& utCache() {
			return m_utCache;
		}

		/// Gets the time supplier.
		auto timeSupplier() const {
			return m_timeSupplier;
		}

		/// Gets the finalization subscriber.
		auto& finalizationSubscriber() const {
			return m_finalizationSubscriber;
		}

		/// Gets the node subscriber.
		auto& nodeSubscriber() const {
			return m_nodeSubscriber;
		}

		/// Gets the state change subscriber.
		auto& stateChangeSubscriber() const {
			return m_stateChangeSubscriber;
		}

		/// Gets the transaction status subscriber.
		auto& transactionStatusSubscriber() const {
			return m_transactionStatusSubscriber;
		}

		/// Gets the (basic) counters.
		/// \note These counters are node counters and do not include counters registered via ServiceLocator.
		const auto& counters() const {
			return m_counters;
		}

		/// Gets the plugin manager.
		const auto& pluginManager() const {
			return m_pluginManager;
		}

		/// Gets the multiservice pool.
		auto& pool() {
			return m_pool;
		}

		/// Gets the tasks.
		auto& tasks() {
			return m_tasks;
		}

		/// Gets the packet handlers.
		auto& packetHandlers() {
			return m_packetHandlers;
		}

		/// Gets the server hooks.
		const auto& hooks() const {
			return m_hooks;
		}

		/// Gets the server hooks.
		auto& hooks() {
			return m_hooks;
		}

		/// Gets the packet io pickers.
		auto& packetIoPickers() {
			return m_packetIoPickers;
		}

	private:
		// references
		const config::BitxorCoreConfiguration& m_config;
		ionet::NodeContainer& m_nodes;
		cache::BitxorCoreCache& m_cache;
		io::BlockStorageCache& m_storage;
		LocalNodeChainScore& m_score;
		const cache::ReadWriteUtCache& m_readWriteUtCache;
		cache::UtCache& m_utCache;
		supplier<Timestamp> m_timeSupplier;

		subscribers::FinalizationSubscriber& m_finalizationSubscriber;
		subscribers::NodeSubscriber& m_nodeSubscriber;
		subscribers::StateChangeSubscriber& m_stateChangeSubscriber;
		subscribers::TransactionStatusSubscriber& m_transactionStatusSubscriber;

		const std::vector<utils::DiagnosticCounter>& m_counters;
		const plugins::PluginManager& m_pluginManager;
		thread::MultiServicePool& m_pool;

		// owned
		std::vector<thread::Task> m_tasks;
		ionet::ServerPacketHandlers m_packetHandlers;
		ServerHooks m_hooks;
		net::PacketIoPickerContainer m_packetIoPickers;
	};

	// endregion

	// region utils

	/// Creates and returns a local finalized height hash pair supplier based on \a state.
	/// \note Result is guaranteed to be in local blockchain storage.
	supplier<model::HeightHashPair> CreateLocalFinalizedHeightHashPairSupplier(const ServiceState& state);

	/// Creates and returns a local finalized height supplier based on \a state.
	/// \note Result is guaranteed to be in local blockchain storage.
	supplier<Height> CreateLocalFinalizedHeightSupplier(const ServiceState& state);

	/// Creates and returns a network finalized height hash pair supplier based on \a state.
	/// \note Result is *NOT* guaranteed to be in local blockchain storage.
	supplier<model::HeightHashPair> CreateNetworkFinalizedHeightHashPairSupplier(const ServiceState& state);

	/// Creates outgoing selector settings based on \a state for \a serviceId and \a requiredRole.
	SelectorSettings CreateOutgoingSelectorSettings(
			const ServiceState& state,
			ionet::ServiceIdentifier serviceId,
			ionet::NodeRoles requiredRole);

	/// Creates incoming selector settings based on \a state for \a serviceId.
	SelectorSettings CreateIncomingSelectorSettings(const ServiceState& state, ionet::ServiceIdentifier serviceId);

	/// Creates a predicate based on \a state that returns \c true when transaction data should be processed.
	predicate<> CreateShouldProcessTransactionsPredicate(const ServiceState& state);

	// endregion
}}
