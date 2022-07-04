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

	/// Namespace plugin configuration settings.
	struct NamespaceConfiguration {
	public:
		/// Maximum namespace name size.
		uint8_t MaxNameSize;

		/// Maximum number of children for a root namespace.
		uint16_t MaxChildNamespaces;

		/// Maximum namespace depth.
		uint8_t MaxNamespaceDepth;

		/// Minimum namespace duration.
		utils::BlockSpan MinNamespaceDuration;

		/// Maximum namespace duration.
		utils::BlockSpan MaxNamespaceDuration;

		/// Root namespace eternal rental fee.
		Amount RootNamespaceEternalFee;

		/// Grace period during which time only the previous owner can renew an expired namespace.
		utils::BlockSpan NamespaceGracePeriodDuration;

		/// Reserved root namespaces that cannot be claimed.
		std::unordered_set<std::string> ReservedRootNamespaceNames;

		/// Address of the namespace rental fee sink account (V1).
		Address NamespaceRentalFeeSinkAddressPOS;

		/// Address of the namespace rental fee sink account (latest).
		Address NamespaceRentalFeeSinkAddress;

		/// Root namespace rental fee per block.
		Amount RootNamespaceRentalFeePerBlock;

		/// Child namespace rental fee.
		Amount ChildNamespaceRentalFee;
		
		/// IsoCode namespace rental fee.
		Amount IsoCodeNamespaceRentalFee;

		/// UnlinkTokenAlias rental fee.
		Amount UnlinkTokenAliasRentalFee;

	private:
		NamespaceConfiguration() = default;

	public:
		/// Creates an uninitialized namespace configuration.
		static NamespaceConfiguration Uninitialized();

		/// Loads a namespace configuration from \a bag.
		static NamespaceConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};

	/// Gets the namespace rental fee sink address from \a config and \a blockchainConfig.
	model::HeightDependentAddress GetNamespaceRentalFeeSinkAddress(
			const NamespaceConfiguration& config,
			const model::BlockchainConfiguration& blockchainConfig);
}}
