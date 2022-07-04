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
#include "VotingKeyPair.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace crypto {

#pragma pack(push, 1)

	/// Two-layer Bellare-Miner signature.
	struct BmTreeSignature {
	public:
		/// Parent public key and signature pair.
		struct ParentPublicKeySignaturePair {
			/// Public key.
			VotingKey ParentPublicKey;

			/// Signature.
			VotingSignature Signature;
		};

	public:
		/// Root pair.
		ParentPublicKeySignaturePair Root;

		/// Bottom pair.
		ParentPublicKeySignaturePair Bottom;

	public:
		/// Returns \c true if this signature is equal to \a rhs.
		bool operator==(const BmTreeSignature& rhs) const;

		/// Returns \c true if this signature is not equal to \a rhs.
		bool operator!=(const BmTreeSignature& rhs) const;
	};

#pragma pack(pop)
}}
