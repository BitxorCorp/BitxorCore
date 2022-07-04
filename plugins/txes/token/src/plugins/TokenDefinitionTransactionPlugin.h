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
#include "bitxorcore/plugins.h"
#include <memory>

namespace bitxorcore { namespace model { class TransactionPlugin; } }

namespace bitxorcore { namespace plugins {

	/// Token rental fee configuration.
	struct TokenRentalFeeConfiguration {
		/// Currency token id.
		UnresolvedTokenId CurrencyTokenId;

		/// Address of the rental fee sink account.
		model::HeightDependentAddress SinkAddress;

		/// Token rental fee.
		Amount Fee;

		/// Public key of the (exempt from fees) genesis account.
		Key GenesisSignerPublicKey;
	};

	/// Creates a token definition transaction plugin given the rental fee configuration (\a config).
	PLUGIN_API
	std::unique_ptr<model::TransactionPlugin> CreateTokenDefinitionTransactionPlugin(const TokenRentalFeeConfiguration& config);
}}
