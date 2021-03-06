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

#include "bitxorcore/config/BitxorCoreKeys.h"
#include "tests/test/crypto/CertificateTestUtils.h"
#include "tests/test/net/CertificateLocator.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace config {

#define TEST_CLASS BitxorCoreKeysTests

	TEST(TEST_CLASS, CanCreateEmpty) {
		// Act:
		BitxorCoreKeys keys;

		// Assert:
		EXPECT_EQ(Key(), keys.caPublicKey());
		EXPECT_EQ(crypto::KeyPair::FromPrivate(crypto::PrivateKey()).publicKey(), keys.nodeKeyPair().publicKey());
	}

	TEST(TEST_CLASS, CanCreateFromComponentKeys) {
		// Arrange:
		auto caPublicKey = test::GenerateRandomByteArray<Key>();
		auto nodeKeyPair = test::GenerateKeyPair();

		// Act:
		BitxorCoreKeys keys(Key(caPublicKey), test::CopyKeyPair(nodeKeyPair));

		// Assert:
		EXPECT_EQ(caPublicKey, keys.caPublicKey());
		EXPECT_EQ(nodeKeyPair.publicKey(), keys.nodeKeyPair().publicKey());
	}

	TEST(TEST_CLASS, CanCreateFromUserConfiguration) {
		// Arrange:
		test::TempDirectoryGuard directoryGuard;
		auto caKeyPair = test::GenerateKeyPair();
		auto nodeKeyPair = test::GenerateKeyPair();

		test::PemCertificate pemCertificate(caKeyPair, nodeKeyPair);
		test::GenerateCertificateDirectory(directoryGuard.name(), pemCertificate);

		// Act:
		BitxorCoreKeys keys(directoryGuard.name());

		// Assert:
		EXPECT_EQ(caKeyPair.publicKey(), keys.caPublicKey());
		EXPECT_EQ(nodeKeyPair.publicKey(), keys.nodeKeyPair().publicKey());
	}

	TEST(TEST_CLASS, GetCaPublicKeyPemFilename_ReturnsCorrectFilename) {
		// Act:
		auto filename = GetCaPublicKeyPemFilename("xyz");

		// Assert:
		EXPECT_EQ("xyz/ca.pubkey.pem", filename);
	}

	TEST(TEST_CLASS, GetNodePrivateKeyPemFilename_ReturnsCorrectFilename) {
		// Act:
		auto filename = GetNodePrivateKeyPemFilename("xyz");

		// Assert:
		EXPECT_EQ("xyz/node.key.pem", filename);
	}
}}
