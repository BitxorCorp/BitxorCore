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

#include "src/state/TokenEntrySerializer.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/core/SerializerTestUtils.h"
#include "tests/test/core/mocks/MockMemoryStream.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenEntrySerializerTests

	// region headers

#pragma pack(push, 1)

	namespace {
		struct TokenEntryHeader {
			bitxorcore::TokenId TokenId;
			Amount Supply;
			bitxorcore::Height Height;
			Address Owner;
			uint32_t Revision;
			model::TokenFlags Flags;
			uint8_t Divisibility;
			BlockDuration Duration;
		};
	}

#pragma pack(pop)

	// endregion

	// region Save

	namespace {
		model::TokenProperties CreateTokenProperties(uint64_t seed) {
			return model::TokenProperties(static_cast<model::TokenFlags>(seed), static_cast<uint8_t>(seed + 1), BlockDuration(seed + 4));
		}

		void AssertEntryHeader(
				const std::vector<uint8_t>& buffer,
				TokenId tokenId,
				Amount supply,
				Height height,
				const Address& owner,
				uint32_t revision,
				uint64_t propertiesSeed) {
			auto message = "entry header at 0";
			const auto& entryHeader = reinterpret_cast<const TokenEntryHeader&>(buffer[0]);

			// - id and supply
			EXPECT_EQ(tokenId, entryHeader.TokenId) << message;
			EXPECT_EQ(supply, entryHeader.Supply) << message;

			// - definition
			EXPECT_EQ(height, entryHeader.Height) << message;
			EXPECT_EQ(owner, entryHeader.Owner) << message;
			EXPECT_EQ(revision, entryHeader.Revision) << message;

			// - properties
			EXPECT_EQ(static_cast<model::TokenFlags>(propertiesSeed), entryHeader.Flags);
			EXPECT_EQ(static_cast<uint8_t>(propertiesSeed + 1), entryHeader.Divisibility);
			EXPECT_EQ(BlockDuration(propertiesSeed + 4), entryHeader.Duration);
		}
	}

	TEST(TEST_CLASS, CanSaveEntry) {
		// Arrange:
		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream stream(buffer);

		auto definition = TokenDefinition(Height(888), test::CreateRandomOwner(), 5, CreateTokenProperties(17));
		auto entry = TokenEntry(TokenId(123), definition);
		entry.increaseSupply(Amount(111));

		// Act:
		TokenEntrySerializer::Save(entry, stream);

		// Assert:
		ASSERT_EQ(sizeof(TokenEntryHeader), buffer.size());
		AssertEntryHeader(buffer, TokenId(123), Amount(111), Height(888), definition.ownerAddress(), 5, 17);
	}

	// endregion

	// region Roundtrip

	TEST(TEST_CLASS, CanRoundtripEntry) {
		// Arrange:
		auto definition = TokenDefinition(Height(888), test::CreateRandomOwner(), 5, CreateTokenProperties(17));
		auto originalEntry = TokenEntry(TokenId(123), definition);
		originalEntry.increaseSupply(Amount(111));

		// Act:
		auto result = test::RunRoundtripBufferTest<TokenEntrySerializer>(originalEntry);

		// Assert:
		test::AssertEqual(originalEntry, result);
	}

	// endregion
}}
