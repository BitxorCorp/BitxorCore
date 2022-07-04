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
#include "bitxorcore/state/AccountBalances.h"

namespace bitxorcore { namespace extensions {

	/// Possible genesis funding types.
	enum class GenesisFundingType {
		/// Unknown (e.g. funding type has not yet been determined).
		Unknown,

		/// Explicitly funded (e.g. token supply transaction).
		Explicit,

		/// Implicitly funded (e.g. balance transfers).
		Implicit
	};

	/// State used by the genesis funding observer.
	struct GenesisFundingState {
	public:
		/// Creates a default genesis funding state.
		GenesisFundingState() : FundingType(GenesisFundingType::Unknown)
		{}

	public:
		/// Total sums of tokens credited in genesis block.
		/// \note Only sum of harvesting token is used. Rest are accumulated for diagnostics.
		state::AccountBalances TotalFundedTokens;

		/// Genesis block funding type.
		GenesisFundingType FundingType;
	};
}}
