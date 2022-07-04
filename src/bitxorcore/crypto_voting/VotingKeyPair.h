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
#include "bitxorcore/crypto/BasicKeyPair.h"
#include "bitxorcore/crypto/SecureByteArray.h"

namespace bitxorcore { namespace crypto {

	struct VotingPrivateKey_tag { static constexpr size_t Size = 32; };
	using VotingPrivateKey = SecureByteArray<VotingPrivateKey_tag>;

	struct VotingSignature_tag { static constexpr size_t Size = 64; };
	using VotingSignature = utils::ByteArray<VotingSignature_tag>;

	/// BLS 381-12 key pair traits.
	struct VotingKeyPairTraits {
	public:
		using PublicKey = VotingKey;
		using PrivateKey = VotingPrivateKey;

	public:
		/// Extracts a public key (\a publicKey) from a private key (\a privateKey).
		static void ExtractPublicKeyFromPrivateKey(const PrivateKey& privateKey, PublicKey& publicKey);
	};

	/// BLS 381-12 key pair.
	/// \note This key pair is used for voting messages.
	using VotingKeyPair = BasicKeyPair<VotingKeyPairTraits>;
}}
