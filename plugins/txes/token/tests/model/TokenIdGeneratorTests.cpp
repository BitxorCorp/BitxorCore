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

#include "src/model/TokenIdGenerator.h"
#include "bitxorcore/crypto/Hashes.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS TokenIdGeneratorTests

	// region GenerateTokenId

	TEST(TEST_CLASS, GenerateTokenId_GeneratesCorrectId) {
		// Arrange:
		auto buffer = std::array<uint8_t, Address::Size + sizeof(uint32_t)>();
		Hash256 zeroHash;
		crypto::Sha3_256(buffer, zeroHash);
		auto expected = reinterpret_cast<const uint64_t&>(zeroHash[0]) & 0x7FFF'FFFF'FFFF'FFFF;

		// Assert:
		EXPECT_EQ(TokenId(expected), GenerateTokenId(Address(), TokenNonce()));
	}

	namespace {
		auto MutateAddress(const Address& address) {
			auto result = address;
			result[0] ^= 0xFF;
			return result;
		}
	}

	TEST(TEST_CLASS, GenerateTokenId_DifferentAddressesProduceDifferentIds) {
		// Arrange:
		auto address1 = test::CreateRandomOwner();
		auto address2 = MutateAddress(address1);

		// Assert:
		EXPECT_NE(GenerateTokenId(address1, TokenNonce()), GenerateTokenId(address2, TokenNonce()));
	}

	TEST(TEST_CLASS, GenerateTokenId_DifferentNoncesProduceDifferentIds) {
		// Arrange:
		auto address = test::CreateRandomOwner();
		auto nonce1 = test::GenerateRandomValue<TokenNonce>();
		auto nonce2 = test::GenerateRandomValue<TokenNonce>();

		// Assert: (could be equal, but rather unlikely for random nonces)
		EXPECT_NE(GenerateTokenId(address, nonce1), GenerateTokenId(address, nonce2));
	}

	TEST(TEST_CLASS, GenerateTokenId_HasHighestBitCleared) {
		for (auto i = 0u; i < 1000; ++i) {
			// Act:
			auto address = test::CreateRandomOwner();
			auto nonce = test::GenerateRandomValue<TokenNonce>();
			auto id = GenerateTokenId(address, nonce);

			// Assert:
			EXPECT_EQ(0u, id.unwrap() >> 63) << address << " " << nonce;
		}
	}

	// endregion
}}
