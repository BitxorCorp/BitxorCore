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
#include "bitxorcore/utils/BlockSpan.h"
#include "bitxorcore/types.h"
#include <stdint.h>

namespace bitxorcore { namespace utils { class ConfigurationBag; } }

namespace bitxorcore { namespace config {

	/// Hash lock plugin configuration settings.
	struct HashLockConfiguration {
	public:
		/// Amount that has to be locked per aggregate in partial cache.
		Amount LockedFundsPerAggregate;

		/// Maximum number of blocks for which a hash lock can exist.
		utils::BlockSpan MaxHashLockDuration;

	private:
		HashLockConfiguration() = default;

	public:
		/// Creates an uninitialized hash lock configuration.
		static HashLockConfiguration Uninitialized();

		/// Loads hash lock configuration from \a bag.
		static HashLockConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};
}}
