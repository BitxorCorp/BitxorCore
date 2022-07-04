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

#include "StateTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	state::BitxorCoreState CreateRandomBitxorCoreState() {
		auto state = state::BitxorCoreState();
		state.LastRecalculationHeight = test::GenerateRandomValue<model::ImportanceHeight>();
		state.LastFinalizedHeight = test::GenerateRandomValue<Height>();
		state.DynamicFeeMultiplier = test::GenerateRandomValue<BlockFeeMultiplier>();
		state.NumTotalTransactions = test::Random();
		return state;
	}

	state::BitxorCoreState CreateDeterministicBitxorCoreState() {
		auto state = state::BitxorCoreState();
		state.LastRecalculationHeight = model::ImportanceHeight(12345);
		state.LastFinalizedHeight = Height(9876);
		state.DynamicFeeMultiplier = BlockFeeMultiplier(334455);
		state.NumTotalTransactions = 7654321;
		return state;
	}

	void AssertEqual(const state::BitxorCoreState& expected, const state::BitxorCoreState& actual, const std::string& message) {
		EXPECT_EQ(expected.LastRecalculationHeight, actual.LastRecalculationHeight) << message;
		EXPECT_EQ(expected.LastFinalizedHeight, actual.LastFinalizedHeight) << message;
		EXPECT_EQ(expected.DynamicFeeMultiplier, actual.DynamicFeeMultiplier) << message;
		EXPECT_EQ(expected.NumTotalTransactions, actual.NumTotalTransactions) << message;
	}
}}
