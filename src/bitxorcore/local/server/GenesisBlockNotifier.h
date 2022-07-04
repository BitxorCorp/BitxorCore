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
#include "bitxorcore/functions.h"

namespace bitxorcore {
	namespace cache { class BitxorCoreCache; }
	namespace io {
		class BlockChangeSubscriber;
		class BlockStorageCache;
	}
	namespace model {
		struct BlockchainConfiguration;
		struct BlockElement;
	}
	namespace plugins { class PluginManager; }
	namespace subscribers {
		class FinalizationSubscriber;
		class StateChangeSubscriber;
	}
}

namespace bitxorcore { namespace local {

	/// Raises genesis block notifications.
	class GenesisBlockNotifier {
	public:
		/// Creates a notifier around \a config, \a cache, \a storage and \a pluginManager.
		GenesisBlockNotifier(
				const model::BlockchainConfiguration& config,
				const cache::BitxorCoreCache& cache,
				const io::BlockStorageCache& storage,
				const plugins::PluginManager& pluginManager);

	public:
		/// Raises and forwards block change notifications to \a subscriber.
		void raise(io::BlockChangeSubscriber& subscriber);

		/// Raises and forwards finalization notifications to \a subscriber.
		void raise(subscribers::FinalizationSubscriber& subscriber);

		/// Raises and forwards state change notifications to \a subscriber.
		void raise(subscribers::StateChangeSubscriber& subscriber);

	private:
		void raise(const consumer<model::BlockElement>& action);

	private:
		const model::BlockchainConfiguration& m_config;
		const cache::BitxorCoreCache& m_cache;
		const io::BlockStorageCache& m_storage;
		const plugins::PluginManager& m_pluginManager;
	};
}}
