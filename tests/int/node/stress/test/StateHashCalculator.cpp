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

#include "StateHashCalculator.h"
#include "bitxorcore/model/Block.h"
#include "tests/int/node/test/LocalNodeGenesisHashTestUtils.h"
#include "tests/test/local/LocalTestUtils.h"

namespace bitxorcore { namespace test {

	StateHashCalculator::StateHashCalculator()
			: m_stateVerificationMode(StateVerificationMode::Disabled)
			, m_config(CreateUninitializedBitxorCoreConfiguration())
			, m_bitxorcoreCache({})
			, m_isDirty(false)
	{}

	StateHashCalculator::StateHashCalculator(const config::BitxorCoreConfiguration& config)
			: m_stateVerificationMode(StateVerificationMode::Enabled)
			, m_config(config)
			, m_pPluginManager(CreatePluginManagerWithRealPlugins(m_config))
			, m_bitxorcoreCache(m_pPluginManager->createCache())
			, m_isDirty(false)
	{}

	const std::string& StateHashCalculator::dataDirectory() const {
		return m_config.User.DataDirectory;
	}

	const config::BitxorCoreConfiguration& StateHashCalculator::config() const {
		return m_config;
	}

	Hash256 StateHashCalculator::execute(const model::Block& block) {
		if (StateVerificationMode::Disabled == m_stateVerificationMode || m_isDirty)
			return Hash256();

		Hash256 blockStateHash;
		auto cacheDelta = m_bitxorcoreCache.createDelta();
		try {
			blockStateHash = CalculateBlockStateHash(block, cacheDelta, *m_pPluginManager);
		} catch (const bitxorcore_runtime_error&) {
			// if state is invalid (e.g. negative balance), zero out state hash and bypass subsequent state hash calculations
			BITXORCORE_LOG(debug)
					<< "block state hash calculation failed at height " << block.Height
					<< ", marking block and remaining chain as dirty";
			blockStateHash = Hash256();
			m_isDirty = true;
		}

		m_bitxorcoreCache.commit(block.Height);

		return blockStateHash;
	}

	void StateHashCalculator::updateStateHash(model::Block& block) {
		block.StateHash = execute(block);
	}
}}
