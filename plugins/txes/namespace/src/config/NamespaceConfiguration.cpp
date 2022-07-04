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

#include "NamespaceConfiguration.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

DEFINE_ADDRESS_CONFIGURATION_VALUE_SUPPORT

namespace bitxorcore { namespace config {

	NamespaceConfiguration NamespaceConfiguration::Uninitialized() {
		return NamespaceConfiguration();
	}

	NamespaceConfiguration NamespaceConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		NamespaceConfiguration config;

#define LOAD_PROPERTY(NAME) utils::LoadIniProperty(bag, "", #NAME, config.NAME)

		LOAD_PROPERTY(MaxNameSize);
		LOAD_PROPERTY(MaxChildNamespaces);
		LOAD_PROPERTY(MaxNamespaceDepth);
		LOAD_PROPERTY(UnlinkTokenAliasRentalFee);
		LOAD_PROPERTY(MinNamespaceDuration);
		LOAD_PROPERTY(MaxNamespaceDuration);
		LOAD_PROPERTY(NamespaceGracePeriodDuration);
		LOAD_PROPERTY(ReservedRootNamespaceNames);
		LOAD_PROPERTY(IsoCodeNamespaceRentalFee);
		LOAD_PROPERTY(NamespaceRentalFeeSinkAddressPOS);
		LOAD_PROPERTY(NamespaceRentalFeeSinkAddress);
		LOAD_PROPERTY(RootNamespaceRentalFeePerBlock);
		LOAD_PROPERTY(ChildNamespaceRentalFee);
		LOAD_PROPERTY(RootNamespaceEternalFee);

#undef LOAD_PROPERTY

		utils::VerifyBagSizeExact(bag, 14);
		return config;
	}

	model::HeightDependentAddress GetNamespaceRentalFeeSinkAddress(
			const NamespaceConfiguration& config,
			const model::BlockchainConfiguration& blockchainConfig) {
		model::HeightDependentAddress sinkAddress(config.NamespaceRentalFeeSinkAddress);
		sinkAddress.trySet(config.NamespaceRentalFeeSinkAddressPOS, blockchainConfig.ForkHeights.TreasuryReissuance);
		return sinkAddress;
	}
}}
