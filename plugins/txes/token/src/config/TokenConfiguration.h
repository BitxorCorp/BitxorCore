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
#include "bitxorcore/model/HeightDependentAddress.h"
#include "bitxorcore/utils/BlockSpan.h"
#include <unordered_set>

namespace bitxorcore {
	namespace model { struct BlockchainConfiguration; }
	namespace utils { class ConfigurationBag; }
}

namespace bitxorcore { namespace config {

	/// Token plugin configuration settings.
	struct TokenConfiguration {
	public:
		/// Maximum number of tokens that an account can own.
		uint16_t MaxTokensPerAccount;

		/// Maximum token duration.
		utils::BlockSpan MaxTokenDuration;

		/// Maximum token divisibility.
		uint8_t MaxTokenDivisibility;

		/// Address of the token rental fee sink account (V1).
		Address TokenRentalFeeSinkAddressPOS;

		/// Address of the token rental fee sink account (latest).
		Address TokenRentalFeeSinkAddress;

		/// Token rental fee.
		Amount TokenRentalFee;

	private:
		TokenConfiguration() = default;

	public:
		/// Creates an uninitialized token configuration.
		static TokenConfiguration Uninitialized();

		/// Loads a token configuration from \a bag.
		static TokenConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};

	/// Gets the token rental fee sink address from \a config and \a blockchainConfig.
	model::HeightDependentAddress GetTokenRentalFeeSinkAddress(
			const TokenConfiguration& config,
			const model::BlockchainConfiguration& blockchainConfig);
}}
