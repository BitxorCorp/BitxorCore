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

#include "LocalNodeTestState.h"
#include "LocalTestUtils.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/state/BitxorCoreState.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/mocks/MockMemoryBlockStorage.h"

namespace bitxorcore { namespace test {

	struct LocalNodeTestState::Impl {
	public:
		Impl(config::BitxorCoreConfiguration&& config, cache::BitxorCoreCache&& cache)
				: m_config(std::move(config))
				, m_cache(std::move(cache))
				, m_storage(std::make_unique<mocks::MockMemoryBlockStorage>(), std::make_unique<mocks::MockMemoryBlockStorage>())
		{}

	public:
		extensions::LocalNodeStateRef ref() {
			return extensions::LocalNodeStateRef(m_config, m_cache, m_storage, m_score);
		}

	private:
		config::BitxorCoreConfiguration m_config;
		cache::BitxorCoreCache m_cache;
		io::BlockStorageCache m_storage;
		extensions::LocalNodeChainScore m_score;
	};

	LocalNodeTestState::LocalNodeTestState() : LocalNodeTestState(CreateEmptyBitxorCoreCache())
	{}

	LocalNodeTestState::LocalNodeTestState(const model::BlockchainConfiguration& config)
			: LocalNodeTestState(config, "", CreateEmptyBitxorCoreCache(config))
	{}

	LocalNodeTestState::LocalNodeTestState(cache::BitxorCoreCache&& cache)
			: m_pImpl(std::make_unique<Impl>(CreatePrototypicalBitxorCoreConfiguration(), std::move(cache)))
	{}

	LocalNodeTestState::LocalNodeTestState(
			const model::BlockchainConfiguration& config,
			const std::string& userDataDirectory,
			cache::BitxorCoreCache&& cache)
			: m_pImpl(std::make_unique<Impl>(
					CreatePrototypicalBitxorCoreConfiguration(model::BlockchainConfiguration(config), userDataDirectory),
					std::move(cache)))
	{}

	LocalNodeTestState::~LocalNodeTestState() = default;

	extensions::LocalNodeStateRef LocalNodeTestState::ref() {
		return m_pImpl->ref();
	}
}}
