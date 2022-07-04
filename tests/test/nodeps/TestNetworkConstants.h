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
#include "bitxorcore/crypto/KeyPair.h"
#include "bitxorcore/exceptions.h"
#include <cstring>

namespace bitxorcore { namespace test {

	// private keys
	constexpr auto Test_Network_Genesis_Private_Key = "B59D53B625CC746ECD478FA24F1ABC80B2031EB7DFCD009D5A74ADE615893175";
	constexpr const char* Test_Network_Private_Keys[] = {
		"FA756667CBE89D6E12B49EB3108D5BDC1AEEF8BDFEE26C47C3202CD2EE26C5B7",
		"8BAEB1D22990A403111564AB8B1B915EEA2079A88C1EAC5A921960EC3681DA41",
		"0B5772CEBF3979F144CAC76C8DC070C8CF3F986A67653DB43A591A3D8059720C",
		"B76DB8484B0D7F017309B5222E14A9B1332B29219C3B9AACCC7D3570884ED2F8",
		"704CF1E32F9B05646BF1F4B7C3781E2741D786F88E53FCAFADFCF361C27E0FC8",
		"3F65C5541311BB2B2CCC35FD872A06F968B5C45859E5D58CEF4A9D14C057E8C9",
		"F0082D6E6C4C6E116FD28C0505556E690B19D8B6960C484581E4992CD8F1629A",
		"83EC604591DCEBCCF09F4ACA3BFC2F55B21C5EA554CBD9F5BAE24C0DB747D41E",
		"49B86B9A092FBB254130175ED7DB2A8D53AC2FF769BC53F44E894850DA3F4639",
		"F387E36E4D86456C935DB257768A9B70B717A11574020DAD836F3FFD72B26C99"
		
	};

	constexpr const char* Test_Network_Vrf_Private_Keys[] = {
		"50C115DB56B10489ED8FD20E4B44E781EC0C86E67BF6BAABCDD2C0B5D85E5CAC",
		"1A2D83C2DF85FCDE03811D56226F465B3D577F2E65EF8FCA429F2CC419B0CDBC",
		"E007221200283911BE22BEB05269CD30F2220A16908661800729D637863BBB4D",
		"B47339447ED3E64591AB0FF3B2E11C0EB52B543D2B9666A98107A0153E6158F6",
		"0BD1BDC5CFE254678BA7C76B76735F3E0A23FB7997E494515B17807FE58C1600",
		"E730074A3A2B5D8098B288CBD58F8D8FEDBC81803279457111A5D7FCEC724227",
		"54BF0B6047235C3D44F97976B3C52B3196659B4CB5A45F5B97488FD2E2A47D98",
		"6292960297B5910485D607E51A64E5946D7074E9F0836ED94E4B3BF8101B2A0B",
		"2273E00BD0A34C67CAEF18B12F2EBF42D55223BC1C95242B280AE85FC1450E88",
		"8FF9B506A2216021852203DBEF64A2018642DA263CC384C70DA3A82409F9BA38",
		"E42F880981CBAC41399E908C8E8C6175D4CC2E8791CBF01E54BC0D790594FD60"
	};

	// addresses
	constexpr auto Token_Rental_Fee_Sink_Address = "TESHM4G4RAQ2CKS6SLV4RSDQXF7Y3CAJLIOAOFA";
	constexpr auto Namespace_Rental_Fee_Sink_Address = "TESHM4G4RAQ2CKS6SLV4RSDQXF7Y3CAJLIOAOFA";

	/// Finds the vrf key pair associated with the specified signing public key (\a signingPublicKey).
	inline crypto::KeyPair LookupVrfKeyPair(const Key& signingPublicKey) {
		auto index = 0u;
		for (const auto* pPrivateKeyString : Test_Network_Vrf_Private_Keys) {
			if (crypto::KeyPair::FromString(Test_Network_Private_Keys[index]).publicKey() == signingPublicKey)
				return crypto::KeyPair::FromString(pPrivateKeyString);

			++index;
		}

		BITXORCORE_THROW_INVALID_ARGUMENT_1("no vrf key pair associated with signing public key", signingPublicKey);
	}

	/// Gets the genesis importance for the specified signing public key (\a signingPublicKey).
	inline Importance GetGenesisImportance(const Key& signingPublicKey) {
		auto index = 0u;
		for (const auto* pPrivateKeyString : Test_Network_Private_Keys) {
			auto keyPair = crypto::KeyPair::FromString(pPrivateKeyString);
			if (keyPair.publicKey() == signingPublicKey)
				break;

			++index;
		}

		// to simulate real harvesting tokens, test genesis block uses discrete importance seedings
		// only first 11 accounts have any harvesting power, two special accounts have importance 4X, nine other 1X
		if (index > 10)
			return Importance(0);

		return 4 == index || 10 == index ? Importance(4'000) : Importance(1'000);
	}
}}
