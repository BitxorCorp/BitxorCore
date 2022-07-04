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
#include "plugins/txes/token/src/model/TokenDefinitionTransaction.h"

namespace bitxorcore { namespace builders {

	/// Builder for a token definition transaction.
	class TokenDefinitionBuilder : public TransactionBuilder {
	public:
		using Transaction = model::TokenDefinitionTransaction;
		using EmbeddedTransaction = model::EmbeddedTokenDefinitionTransaction;

	public:
		/// Creates a token definition builder for building a token definition transaction from \a signer
		/// for the network specified by \a networkIdentifier.
		TokenDefinitionBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer);

	public:
		/// Sets the token duration to \a duration.
		void setDuration(BlockDuration duration);

		/// Sets the token nonce to \a nonce.
		void setNonce(TokenNonce nonce);

		/// Sets the token flags to \a flags.
		void setFlags(model::TokenFlags flags);

		/// Sets the token divisibility to \a divisibility.
		void setDivisibility(uint8_t divisibility);

	public:
		/// Gets the size of token definition transaction.
		/// \note This returns size of a normal transaction not embedded transaction.
		size_t size() const;

		/// Builds a new token definition transaction.
		std::unique_ptr<Transaction> build() const;

		/// Builds a new embedded token definition transaction.
		std::unique_ptr<EmbeddedTransaction> buildEmbedded() const;

	private:
		template<typename TTransaction>
		size_t sizeImpl() const;

		template<typename TTransaction>
		std::unique_ptr<TTransaction> buildImpl() const;

	private:
		TokenId m_id;
		BlockDuration m_duration;
		TokenNonce m_nonce;
		model::TokenFlags m_flags;
		uint8_t m_divisibility;
	};
}}
