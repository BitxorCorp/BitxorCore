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

#include "Accounts.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/exceptions.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/test/nodeps/TestNetworkConstants.h"

namespace bitxorcore { namespace test {

	Accounts::Accounts(size_t numAccounts) {
		if (0 == numAccounts)
			BITXORCORE_THROW_INVALID_ARGUMENT("must create at least one account");

		m_keyPairs.push_back(crypto::KeyPair::FromString(Test_Network_Private_Keys[0]));
		for (auto i = 0u; i < numAccounts - 1; ++i)
			m_keyPairs.push_back(GenerateKeyPair());
	}

	Address Accounts::getAddress(size_t id) const {
		return model::PublicKeyToAddress(getKeyPair(id).publicKey(), model::NetworkIdentifier::Testnet);
	}

	const crypto::KeyPair& Accounts::getKeyPair(size_t id) const {
		if (id >= m_keyPairs.size())
			BITXORCORE_THROW_INVALID_ARGUMENT_1("invalid account id", id);

		return m_keyPairs[id];
	}
}}

