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
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"

namespace bitxorcore {
	namespace model { struct Block; }
	namespace plugins { class PluginManager; }
}

namespace bitxorcore { namespace test {

	/// Calculates state hashes by executing blocks.
	class StateHashCalculator {
	private:
		enum class StateVerificationMode { Enabled, Disabled };

	public:
		/// Creates a default calculator that does not calculate state hashes.
		StateHashCalculator();

		/// Creates a calculator around \a config that calculates state hashes.
		explicit StateHashCalculator(const config::BitxorCoreConfiguration& config);

	public:
		/// Gets the data directory where intermediate data is output.
		const std::string& dataDirectory() const;

		/// Gets the configuration.
		const config::BitxorCoreConfiguration& config() const;

	public:
		/// Executes \a block and returns the block state hash.
		Hash256 execute(const model::Block& block);

		/// Executes \a block and updates its block state hash.
		void updateStateHash(model::Block& block);

	private:
		StateVerificationMode m_stateVerificationMode;
		config::BitxorCoreConfiguration m_config;

		std::shared_ptr<plugins::PluginManager> m_pPluginManager;
		cache::BitxorCoreCache m_bitxorcoreCache;
		bool m_isDirty;
	};
}}
