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
#include "TokenConstants.h"
#include "TokenProperties.h"
#include "TokenTypes.h"
#include "bitxorcore/model/Notifications.h"

namespace bitxorcore { namespace model {

	// region token notification types

/// Defines a token notification type with \a DESCRIPTION, \a CODE and \a CHANNEL.
#define DEFINE_TOKEN_NOTIFICATION(DESCRIPTION, CODE, CHANNEL) DEFINE_NOTIFICATION_TYPE(CHANNEL, Token, DESCRIPTION, CODE)

	/// Token properties were provided.
	DEFINE_TOKEN_NOTIFICATION(Properties, 0x0001, Validator);

	/// Token was defined.
	DEFINE_TOKEN_NOTIFICATION(Definition, 0x0002, All);

	/// Token nonce and id were provided.
	DEFINE_TOKEN_NOTIFICATION(Nonce, 0x0003, Validator);

	/// Token supply was changed.
	DEFINE_TOKEN_NOTIFICATION(Supply_Change, 0x0004, All);

	/// Token rental fee has been sent.
	DEFINE_TOKEN_NOTIFICATION(Rental_Fee, 0x0005, Observer);

#undef DEFINE_TOKEN_NOTIFICATION

	// endregion

	// region TokenPropertiesNotification

	/// Notification of token properties.
	struct TokenPropertiesNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Token_Properties_Notification;

	public:
		/// Creates a notification around \a properties.
		explicit TokenPropertiesNotification(const TokenProperties& properties)
				: Notification(Notification_Type, sizeof(TokenPropertiesNotification))
				, Properties(properties)
		{}

	public:
		/// Token properties.
		TokenProperties Properties;
	};

	// endregion

	// region TokenDefinitionNotification

	/// Notification of a token definition.
	struct TokenDefinitionNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Token_Definition_Notification;

	public:
		/// Creates a notification around \a owner, \a tokenId and \a properties.
		TokenDefinitionNotification(const Address& owner, TokenId tokenId, const TokenProperties& properties)
				: Notification(Notification_Type, sizeof(TokenDefinitionNotification))
				, Owner(owner)
				, TokenId(tokenId)
				, Properties(properties)
		{}

	public:
		/// Token owner.
		Address Owner;

		/// Id of the token.
		bitxorcore::TokenId TokenId;

		/// Token properties.
		TokenProperties Properties;
	};

	// endregion

	// region TokenNonceNotification

	/// Notification of a token nonce and id.
	struct TokenNonceNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Token_Nonce_Notification;

	public:
		/// Creates a notification around \a owner, \a tokenNonce and \a tokenId.
		TokenNonceNotification(const Address& owner, TokenNonce tokenNonce, bitxorcore::TokenId tokenId)
				: Notification(Notification_Type, sizeof(TokenNonceNotification))
				, Owner(owner)
				, TokenNonce(tokenNonce)
				, TokenId(tokenId)
		{}

	public:
		/// Token owner.
		Address Owner;

		/// Token nonce.
		bitxorcore::TokenNonce TokenNonce;

		/// Token id.
		bitxorcore::TokenId TokenId;
	};

	// endregion

	// region TokenSupplyChangeNotification

	/// Notification of a token supply change.
	struct TokenSupplyChangeNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Token_Supply_Change_Notification;

	public:
		/// Creates a notification around \a owner, \a tokenId, \a action and \a delta.
		TokenSupplyChangeNotification(const Address& owner, UnresolvedTokenId tokenId, TokenSupplyChangeAction action, Amount delta)
				: Notification(Notification_Type, sizeof(TokenSupplyChangeNotification))
				, Owner(owner)
				, TokenId(tokenId)
				, Action(action)
				, Delta(delta)
		{}

	public:
		/// Token owner.
		Address Owner;

		/// Id of the affected token.
		UnresolvedTokenId TokenId;

		/// Supply change action.
		TokenSupplyChangeAction Action;

		/// Amount of the change.
		Amount Delta;
	};

	// endregion

	// region rental TokenRentalFeeNotification

	/// Notification of a token rental fee.
	struct TokenRentalFeeNotification : public BasicBalanceNotification<TokenRentalFeeNotification> {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Token_Rental_Fee_Notification;

	public:
		/// Creates a notification around \a sender, \a recipient, \a tokenId and \a amount.
		TokenRentalFeeNotification(
				const Address& sender,
				const ResolvableAddress& recipient,
				UnresolvedTokenId tokenId,
				bitxorcore::Amount amount)
				: BasicBalanceNotification(sender, tokenId, amount)
				, Recipient(recipient)
		{}

	public:
		/// Recipient.
		ResolvableAddress Recipient;
	};

	// endregion
}}
