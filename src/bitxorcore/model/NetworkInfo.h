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
#include "NetworkIdentifier.h"
#include "NodeIdentity.h"
#include "bitxorcore/utils/TimeSpan.h"

namespace bitxorcore { namespace model {

	/// Information about a network.
	struct NetworkInfo {
	public:
		/// Creates a default, uninitialized network info.
		NetworkInfo();

		/// Creates a network info around network \a identifier, node equality strategy (\a nodeEqualityStrategy),
		/// genesis signer public key (\a genesisSignerPublicKey), genesis generation hash seed (\a generationHashSeed)
		/// and genesis epoch time adjustment (\a epochAdjustment).
		NetworkInfo(
				NetworkIdentifier identifier,
				NodeIdentityEqualityStrategy nodeEqualityStrategy,
				const Key& genesisSignerPublicKey,
				const bitxorcore::GenerationHashSeed& generationHashSeed,
				const utils::TimeSpan& epochAdjustment);

	public:
		/// Network identifier.
		NetworkIdentifier Identifier;

		/// Node equality strategy.
		NodeIdentityEqualityStrategy NodeEqualityStrategy;

		/// Genesis public key.
		Key GenesisSignerPublicKey;

		/// Genesis generation hash seed.
		bitxorcore::GenerationHashSeed GenerationHashSeed;

		/// Genesis epoch time adjustment.
		utils::TimeSpan EpochAdjustment;
	};

	/// Gets the genesis signer address for \a networkInfo.
	Address GetGenesisSignerAddress(const NetworkInfo& networkInfo);

	/// Gets the unique network fingerprint for \a networkInfo.
	UniqueNetworkFingerprint GetUniqueNetworkFingerprint(const NetworkInfo& networkInfo);
}}
