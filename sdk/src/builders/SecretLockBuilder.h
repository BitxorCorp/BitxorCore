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
#include "TransactionBuilder.h"
#include "plugins/txes/lock_secret/src/model/SecretLockTransaction.h"

namespace bitxorcore { namespace builders {

	/// Builder for a secret lock transaction.
	class SecretLockBuilder : public TransactionBuilder {
	public:
		using Transaction = model::SecretLockTransaction;
		using EmbeddedTransaction = model::EmbeddedSecretLockTransaction;

	public:
		/// Creates a secret lock builder for building a secret lock transaction from \a signer
		/// for the network specified by \a networkIdentifier.
		SecretLockBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer);

	public:
		/// Sets the locked token recipient address to \a recipientAddress.
		void setRecipientAddress(const UnresolvedAddress& recipientAddress);

		/// Sets the secret to \a secret.
		void setSecret(const Hash256& secret);

		/// Sets the locked token to \a token.
		void setToken(const model::UnresolvedToken& token);

		/// Sets the number of blocks for which a lock should be valid to \a duration.
		void setDuration(BlockDuration duration);

		/// Sets the hash algorithm to \a hashAlgorithm.
		void setHashAlgorithm(model::LockHashAlgorithm hashAlgorithm);

	public:
		/// Gets the size of secret lock transaction.
		/// \note This returns size of a normal transaction not embedded transaction.
		size_t size() const;

		/// Builds a new secret lock transaction.
		std::unique_ptr<Transaction> build() const;

		/// Builds a new embedded secret lock transaction.
		std::unique_ptr<EmbeddedTransaction> buildEmbedded() const;

	private:
		template<typename TTransaction>
		size_t sizeImpl() const;

		template<typename TTransaction>
		std::unique_ptr<TTransaction> buildImpl() const;

	private:
		UnresolvedAddress m_recipientAddress;
		Hash256 m_secret;
		model::UnresolvedToken m_token;
		BlockDuration m_duration;
		model::LockHashAlgorithm m_hashAlgorithm;
	};
}}
