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

#include "SharedKey.h"
#include "CryptoUtils.h"
#include "Hashes.h"
#include "SecureZero.h"
#include <cstring>

namespace bitxorcore { namespace crypto {

	SharedKey Hkdf_Hmac_Sha256_32(const Key& sharedSecret) {
		Hash256 salt;
		Hash256 pseudoRandomKey;
		Hmac_Sha256(salt, sharedSecret, pseudoRandomKey);

		// specialized for single repetition, last byte contains counter value
		constexpr auto Buffer_Length = 8 + 1;
		std::array<uint8_t, Buffer_Length> buffer{ { 0x62, 0x69, 0x74, 0x78, 0x6F, 0x72, 0x6E, 0x74, 0x01 } };

		Hash256 outputKeyingMaterial;
		Hmac_Sha256(pseudoRandomKey, buffer, outputKeyingMaterial);

		auto sharedKey = outputKeyingMaterial.copyTo<SharedKey>();
		SecureZero(pseudoRandomKey);
		SecureZero(outputKeyingMaterial);
		return sharedKey;
	}

	Key DeriveSharedSecret(const KeyPair& keyPair, const Key& otherPublicKey) {
		ScalarMultiplier multiplier;
		ExtractMultiplier(keyPair.privateKey(), multiplier);

		Key sharedSecret;
		if (!ScalarMult(multiplier, otherPublicKey, sharedSecret))
			return Key();

		SecureZero(multiplier);
		return sharedSecret;
	}

	SharedKey DeriveSharedKey(const KeyPair& keyPair, const Key& otherPublicKey) {
		auto sharedSecret = DeriveSharedSecret(keyPair, otherPublicKey);
		if (Key() == sharedSecret)
			return SharedKey();

		auto sharedKey = Hkdf_Hmac_Sha256_32(sharedSecret);
		SecureZero(sharedSecret);
		return sharedKey;
	}
}}
