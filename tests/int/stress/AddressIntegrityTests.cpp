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

#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/test/nodeps/TestNetworkConstants.h"
#include "tests/TestHarness.h"

namespace bitxorcore {

#define TEST_CLASS AddressIntegrityTests

	TEST(TEST_CLASS, CanFindAddressStartingWithTA) {
		// Arrange:
		for (auto i = 0u; i < 1000; ++i) {
			auto kp = test::GenerateKeyPair();
			auto rawAddress = model::PublicKeyToAddress(kp.publicKey(), model::NetworkIdentifier::Testnet);
			auto address = model::AddressToString(rawAddress);
			if (address[0] == 'T' && address[1] == 'E')
				return;
		}

		FAIL() << "could not generate TES* address";
	}

	namespace {
		struct TestnetTraits {
			static constexpr auto Network_Identifier = model::NetworkIdentifier::Testnet;
			static constexpr auto Genesis_Private_Key = test::Test_Network_Genesis_Private_Key;
			static constexpr auto Expected_Genesis_Address = "TESCFUBRZL5TTE5VP7XFVXAL2QBTUQY7X6Q6R7I";

			static std::vector<const char*> PrivateKeys() {
				return std::vector<const char*>(&test::Test_Network_Private_Keys[0], &test::Test_Network_Private_Keys[9]);
			}

			static std::vector<std::string> ExpectedAddresses() {
				return {
					"TESEJH2T5SO7XRMNYSTJJU3EFR45GOG2QM2XWFI",
					"TESKCK3ILWYVROUGQ3DW6OE2UQHN4RDH3BVQMQQ",
					"TESHPVFVKHZG6AQJHDAIVKM2U6BNYYCMYTKRGNY",
					"TESJTXU3R3WRRSI4IGSOLIDTPOK7OPRTFXUWYMI",
					"TESP4ZN2VYO7B7EWMTFP2Y3IABT3VBGVT772N4I",
					"TESON45Q5QX7Z6H6PJ3RW3CK4KT4FUQ55QTVEYA",
					"TESECYV6JILLHLLH2JRBZG27JQBFFCCJPVS34NI",
					"TESBHFCVDPRDKANEVYBOOE3TIQ22EE7QX6WI7TY",
					"TESB3LIVOMKYLSAOQ3RP7CUQUQW66DAKRKJ6IMA",
					
					"TESMR3KPXZCAFFII6TUGYUV3O6UU47IAISJ3SUQ"
				};
			}
		};

		auto PrivateKeyStringToAddressString(const std::string& pkString, model::NetworkIdentifier networkIdentifier) {
			auto kp = crypto::KeyPair::FromString(pkString);
			auto rawAddress = model::PublicKeyToAddress(kp.publicKey(), networkIdentifier);
			return model::AddressToString(rawAddress);
		}
	}

#define TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Testnet) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TestnetTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	TRAITS_BASED_TEST(GenesisKeyProducesExpectedAddress) {
		// Act:
		auto address = PrivateKeyStringToAddressString(TTraits::Genesis_Private_Key, TTraits::Network_Identifier);

		// Assert:
		EXPECT_EQ(TTraits::Expected_Genesis_Address, address);
	}

	TRAITS_BASED_TEST(PrivateKeysProduceExpectedAddresses) {
		// Arrange:
		auto expectedAddresses = TTraits::ExpectedAddresses();

		// Act + Assert:
		auto i = 0u;
		for (const auto& pkString : TTraits::PrivateKeys()) {
			const auto& expectedAddress = expectedAddresses[i++];
			EXPECT_EQ(expectedAddress, PrivateKeyStringToAddressString(pkString, TTraits::Network_Identifier));
		}
	}
}
