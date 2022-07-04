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

	/// Secret lock plugin configuration settings.
	struct SecretLockConfiguration {
	public:
		/// Maximum number of blocks for which a secret lock can exist.
		utils::BlockSpan MaxSecretLockDuration;

		/// Minimum size of a proof in bytes.
		uint16_t MinProofSize;

		/// Maximum size of a proof in bytes.
		uint16_t MaxProofSize;

	private:
		SecretLockConfiguration() = default;

	public:
		/// Creates an uninitialized lock configuration.
		static SecretLockConfiguration Uninitialized();

		/// Loads lock configuration from \a bag.
		static SecretLockConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};
}}
