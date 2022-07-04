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

namespace bitxorcore {
	namespace cache { class BitxorCoreCache; }
	namespace config { class BitxorCoreConfiguration; }
	namespace extensions { class LocalNodeChainScore; }
	namespace io { class BlockStorageCache; }
}

namespace bitxorcore { namespace extensions {

	/// Reference to a local node's basic state.
	struct LocalNodeStateRef {
	public:
		/// Creates a local node state ref referencing state composed of
		/// \a config, \a cache, \a storage and \a score.
		LocalNodeStateRef(
				const config::BitxorCoreConfiguration& config,
				cache::BitxorCoreCache& cache,
				io::BlockStorageCache& storage,
				LocalNodeChainScore& score)
				: Config(config)
				, Cache(cache)
				, Storage(storage)
				, Score(score)
		{}

	public:
		/// BitxorCore configuration.
		const config::BitxorCoreConfiguration& Config;

		/// Local node cache.
		cache::BitxorCoreCache& Cache;

		/// Local node storage.
		io::BlockStorageCache& Storage;

		/// Local node score.
		LocalNodeChainScore& Score;
	};
}}
