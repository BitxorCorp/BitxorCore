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

#include "TokenConfiguration.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

DEFINE_ADDRESS_CONFIGURATION_VALUE_SUPPORT

namespace bitxorcore { namespace config {

	TokenConfiguration TokenConfiguration::Uninitialized() {
		return TokenConfiguration();
	}

	TokenConfiguration TokenConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		TokenConfiguration config;

#define LOAD_PROPERTY(NAME) utils::LoadIniProperty(bag, "", #NAME, config.NAME)

		LOAD_PROPERTY(MaxTokensPerAccount);
		LOAD_PROPERTY(MaxTokenDuration);
		LOAD_PROPERTY(MaxTokenDivisibility);

		LOAD_PROPERTY(TokenRentalFeeSinkAddressPOS);
		LOAD_PROPERTY(TokenRentalFeeSinkAddress);
		LOAD_PROPERTY(TokenRentalFee);

#undef LOAD_PROPERTY

		utils::VerifyBagSizeExact(bag, 6);
		return config;
	}

	model::HeightDependentAddress GetTokenRentalFeeSinkAddress(
			const TokenConfiguration& config,
			const model::BlockchainConfiguration& blockchainConfig) {
		model::HeightDependentAddress sinkAddress(config.TokenRentalFeeSinkAddress);
		sinkAddress.trySet(config.TokenRentalFeeSinkAddressPOS, blockchainConfig.ForkHeights.TreasuryReissuance);
		return sinkAddress;
	}
}}
