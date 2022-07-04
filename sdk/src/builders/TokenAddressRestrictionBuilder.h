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
#include "plugins/txes/restriction_token/src/model/TokenAddressRestrictionTransaction.h"

namespace bitxorcore { namespace builders {

	/// Builder for a token address restriction transaction.
	class TokenAddressRestrictionBuilder : public TransactionBuilder {
	public:
		using Transaction = model::TokenAddressRestrictionTransaction;
		using EmbeddedTransaction = model::EmbeddedTokenAddressRestrictionTransaction;

	public:
		/// Creates a token address restriction builder for building a token address restriction transaction from \a signer
		/// for the network specified by \a networkIdentifier.
		TokenAddressRestrictionBuilder(model::NetworkIdentifier networkIdentifier, const Key& signer);

	public:
		/// Sets the identifier of the token to which the restriction applies to \a tokenId.
		void setTokenId(UnresolvedTokenId tokenId);

		/// Sets the restriction key to \a restrictionKey.
		void setRestrictionKey(uint64_t restrictionKey);

		/// Sets the previous restriction value to \a previousRestrictionValue.
		void setPreviousRestrictionValue(uint64_t previousRestrictionValue);

		/// Sets the new restriction value to \a newRestrictionValue.
		void setNewRestrictionValue(uint64_t newRestrictionValue);

		/// Sets the address being restricted to \a targetAddress.
		void setTargetAddress(const UnresolvedAddress& targetAddress);

	public:
		/// Gets the size of token address restriction transaction.
		/// \note This returns size of a normal transaction not embedded transaction.
		size_t size() const;

		/// Builds a new token address restriction transaction.
		std::unique_ptr<Transaction> build() const;

		/// Builds a new embedded token address restriction transaction.
		std::unique_ptr<EmbeddedTransaction> buildEmbedded() const;

	private:
		template<typename TTransaction>
		size_t sizeImpl() const;

		template<typename TTransaction>
		std::unique_ptr<TTransaction> buildImpl() const;

	private:
		UnresolvedTokenId m_tokenId;
		uint64_t m_restrictionKey;
		uint64_t m_previousRestrictionValue;
		uint64_t m_newRestrictionValue;
		UnresolvedAddress m_targetAddress;
	};
}}
