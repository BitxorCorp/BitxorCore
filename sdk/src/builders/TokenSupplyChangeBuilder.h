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
#include "plugins/txes/token/src/model/TokenSupplyChangeTransaction.h"

namespace bitxorcore { namespace builders {

	/// Builder for a token supply change transaction.
	class TokenSupplyChangeBuilder : public TransactionBuilder {
	public:
		using Transaction = model::TokenSupplyChangeTransaction;
		using EmbeddedTransaction = model::EmbeddedTokenSupplyChangeTransaction;

	public:
		/// Creates a token supply change builder for building a token supply change transaction from \a signer
		/// for the network specified by \a networkIdentifier.
		TokenSupplyChangeBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer);

	public:
		/// Sets the affected token identifier to \a tokenId.
		void setTokenId(UnresolvedTokenId tokenId);

		/// Sets the change amount to \a delta.
		void setDelta(Amount delta);

		/// Sets the supply change action to \a action.
		void setAction(model::TokenSupplyChangeAction action);

	public:
		/// Gets the size of token supply change transaction.
		/// \note This returns size of a normal transaction not embedded transaction.
		size_t size() const;

		/// Builds a new token supply change transaction.
		std::unique_ptr<Transaction> build() const;

		/// Builds a new embedded token supply change transaction.
		std::unique_ptr<EmbeddedTransaction> buildEmbedded() const;

	private:
		template<typename TTransaction>
		size_t sizeImpl() const;

		template<typename TTransaction>
		std::unique_ptr<TTransaction> buildImpl() const;

	private:
		UnresolvedTokenId m_tokenId;
		Amount m_delta;
		model::TokenSupplyChangeAction m_action;
	};
}}
