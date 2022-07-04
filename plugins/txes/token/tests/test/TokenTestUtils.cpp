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

#include "TokenTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	Address CreateRandomOwner() {
		return GenerateRandomByteArray<Address>();
	}

	model::TokenProperties CreateTokenPropertiesFromValues(uint8_t flags, uint8_t divisibility, uint64_t duration) {
		return model::TokenProperties(static_cast<model::TokenFlags>(flags), divisibility, BlockDuration(duration));
	}

	model::TokenProperties CreateTokenPropertiesWithDuration(BlockDuration duration) {
		return CreateTokenPropertiesFromValues(0, 0, duration.unwrap());
	}

	state::TokenDefinition CreateTokenDefinition(Height height) {
		return state::TokenDefinition(height, CreateRandomOwner(), 3, model::TokenProperties());
	}

	state::TokenEntry CreateTokenEntry(TokenId id, Amount supply) {
		return CreateTokenEntry(id, Height(987), supply);
	}

	state::TokenEntry CreateTokenEntry(TokenId id, Height height, Amount supply) {
		auto entry = state::TokenEntry(id, CreateTokenDefinition(height));
		entry.increaseSupply(supply);
		return entry;
	}

	namespace {
		state::TokenDefinition CreateTokenDefinition(Height height, const Address& owner, BlockDuration duration) {
			return state::TokenDefinition(height, owner, 3, CreateTokenPropertiesWithDuration(duration));
		}
	}

	state::TokenEntry CreateTokenEntry(TokenId id, Height height, const Address& owner, Amount supply, BlockDuration duration) {
		auto entry = state::TokenEntry(id, CreateTokenDefinition(height, owner, duration));
		entry.increaseSupply(supply);
		return entry;
	}

	namespace {
		auto ToTokenFlag(uint8_t value) {
			return static_cast<model::TokenFlags>(1 << value);
		}
	}

	void AssertTokenDefinitionProperties(
			const model::TokenProperties& expectedProperties,
			const model::TokenProperties& actualProperties) {
		for (uint8_t i = 0u; i < 8; ++i)
			EXPECT_EQ(expectedProperties.is(ToTokenFlag(i)), actualProperties.is(ToTokenFlag(i))) << "bit " << i;

		EXPECT_EQ(expectedProperties.divisibility(), actualProperties.divisibility());
		EXPECT_EQ(expectedProperties.duration(), actualProperties.duration());
	}

	namespace {
		void AssertEqual(const state::TokenDefinition& expected, const state::TokenDefinition& actual) {
			EXPECT_EQ(expected.startHeight(), actual.startHeight());
			EXPECT_EQ(expected.ownerAddress(), actual.ownerAddress());
			AssertTokenDefinitionProperties(expected.properties(), actual.properties());
		}
	}

	void AssertEqual(const state::TokenEntry& expected, const state::TokenEntry& actual) {
		EXPECT_EQ(expected.tokenId(), actual.tokenId());
		EXPECT_EQ(expected.supply(), actual.supply());
		AssertEqual(expected.definition(), actual.definition());
	}
}}
