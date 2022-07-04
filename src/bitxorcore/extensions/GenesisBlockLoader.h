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
#include "GenesisFundingState.h"
#include "bitxorcore/model/GenesisNotificationPublisher.h"
#include "bitxorcore/observers/ObserverTypes.h"
#include "bitxorcore/functions.h"

namespace bitxorcore {
	namespace cache { class BitxorCoreCacheDelta; }
	namespace extensions { struct LocalNodeStateRef; }
	namespace model {
		struct BlockchainConfiguration;
		struct BlockElement;
	}
	namespace plugins { class PluginManager; }
}

namespace bitxorcore { namespace extensions {

	/// State hash verification.
	enum class StateHashVerification {
		/// State hash verification disabled.
		Disabled,

		/// State hash verification enabled.
		Enabled
	};

	/// Loads and executes a genesis block.
	class GenesisBlockLoader {
	public:
		/// Creates a loader around \a cacheDelta, \a pluginManager and \a pObserver.
		GenesisBlockLoader(
				cache::BitxorCoreCacheDelta& cacheDelta,
				const plugins::PluginManager& pluginManager,
				std::unique_ptr<const observers::NotificationObserver>&& pObserver);

	public:
		/// Loads the genesis block from storage, updates state in \a stateRef and verifies state hash (\a stateHashVerification).
		void execute(const LocalNodeStateRef& stateRef, StateHashVerification stateHashVerification);

		/// Loads the genesis block from storage, updates state in \a stateRef optionally verifying state hash (\a stateHashVerification)
		/// and commits all changes to cache.
		void executeAndCommit(
				const LocalNodeStateRef& stateRef,
				StateHashVerification stateHashVerification = StateHashVerification::Enabled);

		/// Executes the genesis block (\a genesisBlockElement), applies all changes to cache delta and checks consistency
		/// against \a config.
		/// \note Execution uses a default bitxorcore state.
		void execute(const model::BlockchainConfiguration& config, const model::BlockElement& genesisBlockElement);

	private:
		enum class Verbosity { Off, On };

		void validateStateless(const model::WeakEntityInfos& entityInfos) const;

		void validateStatefulAndObserve(
				Timestamp timestamp,
				const model::WeakEntityInfos& entityInfos,
				observers::ObserverState& observerState) const;

		void execute(
				const model::BlockchainConfiguration& config,
				const model::BlockElement& genesisBlockElement,
				StateHashVerification stateHashVerification,
				Verbosity verbosity);

	private:
		cache::BitxorCoreCacheDelta& m_cacheDelta;
		const plugins::PluginManager& m_pluginManager;
		Address m_genesisAddress;
		GenesisFundingState m_genesisFundingState;
		std::shared_ptr<const observers::AggregateNotificationObserver> m_pNotificationObserver;
		model::GenesisNotificationPublisherOptions m_publisherOptions;
	};
}}
