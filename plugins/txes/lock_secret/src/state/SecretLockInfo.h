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
#include "src/model/LockHashAlgorithm.h"
#include "src/model/LockHashUtils.h"
#include "plugins/txes/lock_shared/src/state/LockInfo.h"
#include "bitxorcore/plugins.h"

namespace bitxorcore { namespace state {

	/// Secret lock info.
	struct PLUGIN_API_DEPENDENCY SecretLockInfo : public LockInfo {
	public:
		/// Creates a default secret lock info.
		SecretLockInfo() : LockInfo()
		{}

		/// Creates a secret lock info around \a ownerAddress, \a tokenId, \a amount, \a endHeight, \a hashAlgorithm, \a secret
		/// and \a recipientAddress.
		SecretLockInfo(
				const Address& ownerAddress,
				bitxorcore::TokenId tokenId,
				bitxorcore::Amount amount,
				Height endHeight,
				model::LockHashAlgorithm hashAlgorithm,
				const Hash256& secret,
				const bitxorcore::Address& recipientAddress)
				: LockInfo(ownerAddress, tokenId, amount, endHeight)
				, HashAlgorithm(hashAlgorithm)
				, Secret(secret)
				, RecipientAddress(recipientAddress)
				, CompositeHash()
		{}

	public:
		/// Hash algorithm.
		model::LockHashAlgorithm HashAlgorithm;

		/// Secret.
		Hash256 Secret;

		/// Locked token recipient address.
		Address RecipientAddress;

		/// Composite hash.
		Hash256 CompositeHash;
	};

	/// Gets the lock identifier for \a lockInfo.
	inline const Hash256& GetLockIdentifier(const SecretLockInfo& lockInfo) {
		return lockInfo.CompositeHash;
	}
}}
