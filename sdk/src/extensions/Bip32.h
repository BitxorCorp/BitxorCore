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

namespace bitxorcore { namespace extensions {

	/// Bitxor BIP32 node.
	class Bip32Node {
	private:
		Bip32Node(const RawBuffer& key, const RawBuffer& data);

	private:
		Bip32Node(Hash512&& hmacResult);

	public:
		/// Gets the node's chain code.
		const Hash256& chainCode() const;

		/// Gets the node's public key.
		const Key& publicKey() const;

	public:
		/// Derives a direct child node with \a id.
		Bip32Node derive(uint32_t id);

		/// Derives a descendent node with \a path.
		Bip32Node derive(const std::vector<uint32_t>& path);

	public:
		/// Creates a BIP32 root node from \a seed.
		static Bip32Node FromSeed(const RawBuffer& seed);

		/// Extracts the key pair from \a node.
		static crypto::KeyPair ExtractKeyPair(Bip32Node&& node);

	private:
		crypto::KeyPair m_keyPair;
		Hash256 m_chainCode;
	};
}}
