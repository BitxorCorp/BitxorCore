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
#include "src/state/TokenEntry.h"

namespace bitxorcore { namespace test {

	/// Creates a random token owner.
	Address CreateRandomOwner();

	/// Creates token properties from values: \a flags, \a divisibility, \a duration.
	model::TokenProperties CreateTokenPropertiesFromValues(uint8_t flags, uint8_t divisibility, uint64_t duration);

	/// Creates token properties with a custom \a duration.
	model::TokenProperties CreateTokenPropertiesWithDuration(BlockDuration duration);

	/// Creates a token definition with \a height.
	state::TokenDefinition CreateTokenDefinition(Height height);

	/// Creates a token entry with \a id and \a supply.
	state::TokenEntry CreateTokenEntry(TokenId id, Amount supply);

	/// Creates a token entry with \a id, \a height and \a supply.
	state::TokenEntry CreateTokenEntry(TokenId id, Height height, Amount supply);

	/// Creates a token entry around \a id, \a height, \a owner, \a supply and \a duration.
	state::TokenEntry CreateTokenEntry(TokenId id, Height height, const Address& owner, Amount supply, BlockDuration duration);

	/// Asserts that actual properties (\a actualProperties) exactly match expected properties (\a expectedProperties).
	void AssertTokenDefinitionProperties(
			const model::TokenProperties& expectedProperties,
			const model::TokenProperties& actualProperties);

	/// Asserts that token entry \a actual is equal to \a expected.
	void AssertEqual(const state::TokenEntry& expected, const state::TokenEntry& actual);
}}
