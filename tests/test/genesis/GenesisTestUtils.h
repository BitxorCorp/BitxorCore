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
#include "bitxorcore/types.h"

namespace bitxorcore { namespace cache { class BitxorCoreCacheView; } }

namespace bitxorcore { namespace test {

	/// Converts a raw private key string (\a privateKeyString) to a public key.
	Key RawPrivateKeyToPublicKey(const char* privateKeyString);

	/// Converts a raw private key string (\a privateKeyString) to an address.
	Address RawPrivateKeyToAddress(const char* privateKeyString);

	/// Converts a raw public key string (\a publicKeyString) to an address.
	Address RawPublicKeyToAddress(const char* publicKeyString);

	/// Asserts that \a view contains expected genesis account state.
	void AssertGenesisAccountState(const cache::BitxorCoreCacheView& view);

	/// Asserts that \a view contains expected genesis token state.
	void AssertGenesisTokenState(const cache::BitxorCoreCacheView& view);

	/// Asserts that \a view contains expected genesis namespace state.
	void AssertGenesisNamespaceState(const cache::BitxorCoreCacheView& view);
}}
