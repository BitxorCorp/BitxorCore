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
#include "bitxorcore/model/Token.h"
#include "bitxorcore/model/Notifications.h"

namespace bitxorcore { namespace model {

	// region transfer notification types

/// Defines a transfer notification type with \a DESCRIPTION, \a CODE and \a CHANNEL.
#define DEFINE_TRANSFER_NOTIFICATION(DESCRIPTION, CODE, CHANNEL) DEFINE_NOTIFICATION_TYPE(CHANNEL, Transfer, DESCRIPTION, CODE)

	/// Transfer was received with a message.
	DEFINE_TRANSFER_NOTIFICATION(Message, 0x0001, All);

	/// Transfer was received with at least one token.
	DEFINE_TRANSFER_NOTIFICATION(Tokens, 0x0002, Validator);

#undef DEFINE_TRANSFER_NOTIFICATION

	// endregion

	// region TransferMessageNotification

	/// Notification of a transfer transaction with a message.
	struct TransferMessageNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Transfer_Message_Notification;

	public:
		/// Creates a notification around \a senderPublicKey, \a recipient, \a messageSize and \a pMessage.
		TransferMessageNotification(
				const Key& senderPublicKey,
				const UnresolvedAddress& recipient,
				uint16_t messageSize,
				const uint8_t* pMessage)
				: Notification(Notification_Type, sizeof(TransferMessageNotification))
				, SenderPublicKey(senderPublicKey)
				, Recipient(recipient)
				, MessageSize(messageSize)
				, MessagePtr(pMessage)
		{}

	public:
		/// Message sender public key.
		const Key& SenderPublicKey;

		/// Message recipient.
		const UnresolvedAddress& Recipient;

		/// Message size in bytes.
		uint16_t MessageSize;

		/// Const pointer to the message data.
		const uint8_t* MessagePtr;
	};

	// endregion

	// region TransferTokensNotification

	/// Notification of a transfer transaction with tokens.
	struct TransferTokensNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = Transfer_Tokens_Notification;

	public:
		/// Creates a notification around \a tokensCount and \a pTokens.
		TransferTokensNotification(uint8_t tokensCount, const UnresolvedToken* pTokens)
				: Notification(Notification_Type, sizeof(TransferTokensNotification))
				, TokensCount(tokensCount)
				, TokensPtr(pTokens)
		{}

	public:
		/// Number of tokens.
		uint8_t TokensCount;

		/// Const pointer to the first token.
		const UnresolvedToken* TokensPtr;
	};

	// endregion
}}
