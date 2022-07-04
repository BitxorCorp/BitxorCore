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

#include "ToolKeys.h"
#include "Random.h"
#include "bitxorcore/crypto/Hashes.h"
#include "bitxorcore/model/Address.h"
#include <random>

namespace bitxorcore { namespace tools {

	namespace {
		void NextKey(Key& key) {
			Hash256 hash;
			crypto::Sha3_256(key, hash);
			std::copy(hash.cbegin(), hash.cend(), key.begin());
		}

		auto GetDeterministicKey(const Key& baseKey, uint64_t keyId) {
			auto privateKey = baseKey;
			*reinterpret_cast<uint64_t*>(privateKey.data()) = keyId;

			Hash256 hash;
			crypto::Sha3_256(privateKey, hash);
			std::copy(hash.cbegin(), hash.cend(), privateKey.begin());
			return privateKey;
		}
	}

	crypto::KeyPair GenerateRandomKeyPair() {
		return crypto::KeyPair::FromPrivate(crypto::PrivateKey::Generate(RandomByte));
	}

	crypto::KeyPair GetDeterministicKeyPair(const Key& baseKey, uint64_t keyId) {
		auto key = GetDeterministicKey(baseKey, keyId);
		return crypto::KeyPair::FromPrivate(crypto::PrivateKey::FromBuffer(key));
	}

	crypto::KeyPair CopyKeyPair(const crypto::KeyPair& keyPair) {
		return crypto::KeyPair::FromPrivate(crypto::PrivateKey::FromBuffer(keyPair.privateKey()));
	}

	std::vector<Address> PrepareAddresses(size_t count) {
		std::vector<Address> addresses;
		auto seedKey = Key();

		addresses.reserve(count);
		while (count != addresses.size()) {
			NextKey(seedKey);
			auto address = model::PublicKeyToAddress(seedKey, model::NetworkIdentifier::Testnet);

			// just to have addresses starting with 'SA'
			if (0 == (address[1] & 0xF8))
				addresses.push_back(address);
		}

		return addresses;
	}
}}
