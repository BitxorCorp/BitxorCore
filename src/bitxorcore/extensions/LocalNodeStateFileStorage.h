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
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/types.h"

namespace bitxorcore {
	namespace cache {
		class CacheStorage;
		class BitxorCoreCache;
		class BitxorCoreCacheDelta;
		struct SupplementalData;
	}
	namespace config { struct NodeConfiguration; }
	namespace extensions { struct LocalNodeStateRef; }
	namespace model { class ChainScore; }
	namespace plugins { class PluginManager; }
}

namespace bitxorcore { namespace extensions {

	/// Information about state heights.
	struct StateHeights {
		/// Cache height.
		Height Cache;

		/// Storage height.
		Height Storage;
	};

	/// Returns \c true if serialized state is present in \a directory.
	bool HasSerializedState(const config::BitxorCoreDirectory& directory);

	/// Loads dependent state from \a directory and updates \a cache.
	void LoadDependentStateFromDirectory(const config::BitxorCoreDirectory& directory, cache::BitxorCoreCache& cache);

	/// Loads bitxorcore state into \a stateRef from \a directory given \a pluginManager.
	StateHeights LoadStateFromDirectory(
			const config::BitxorCoreDirectory& directory,
			const LocalNodeStateRef& stateRef,
			const plugins::PluginManager& pluginManager);

	/// Serializes local node state.
	class LocalNodeStateSerializer {
	public:
		/// Creates a serializer around specified \a directory.
		explicit LocalNodeStateSerializer(const config::BitxorCoreDirectory& directory);

	public:
		/// Saves state composed of \a cache and \a score.
		void save(const cache::BitxorCoreCache& cache, const model::ChainScore& score) const;

		/// Saves state composed of \a cacheDelta, \a score and \a height using \a cacheStorages.
		void save(
				const cache::BitxorCoreCacheDelta& cacheDelta,
				const std::vector<std::unique_ptr<const cache::CacheStorage>>& cacheStorages,
				const model::ChainScore& score,
				Height height) const;

		/// Moves serialized state to \a destinationDirectory.
		void moveTo(const config::BitxorCoreDirectory& destinationDirectory);

	private:
		config::BitxorCoreDirectory m_directory;
	};

	/// Serializes state composed of \a cache and \a score with checkpointing to \a dataDirectory given \a nodeConfig.
	void SaveStateToDirectoryWithCheckpointing(
			const config::BitxorCoreDataDirectory& dataDirectory,
			const config::NodeConfiguration& nodeConfig,
			const cache::BitxorCoreCache& cache,
			const model::ChainScore& score);
}}
