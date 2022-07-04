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
#include "src/model/TokenRestrictionTypes.h"
#include "bitxorcore/model/Notifications.h"

namespace bitxorcore { namespace model {

	// region token restriction notification types

/// Defines a token restriction notification type with \a DESCRIPTION, \a CODE and \a CHANNEL.
#define DEFINE_TOKEN_RESTRICTION_NOTIFICATION(DESCRIPTION, CODE, CHANNEL) \
	DEFINE_NOTIFICATION_TYPE(CHANNEL, RestrictionToken, DESCRIPTION, CODE)

	/// Token restriction type.
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Type, 0x0001, Validator);

	/// Token restriction is required to exist.
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Required, 0x0002, Validator);

	/// Token global restriction modification (previous value).
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Global_Restriction_Previous, 0x0003, All);

	/// Token global restriction modification (new value).
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Global_Restriction_New, 0x0004, All);

	/// Token address restriction modification (previous value).
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Address_Restriction_Previous, 0x0005, All);

	/// Token address restriction modification (new value).
	DEFINE_TOKEN_RESTRICTION_NOTIFICATION(Address_Restriction_New, 0x0006, All);

#undef DEFINE_TOKEN_RESTRICTION_NOTIFICATION

	// endregion

	// region TokenRestrictionTypeNotification

	/// Notification of a token restriction type.
	struct TokenRestrictionTypeNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = RestrictionToken_Type_Notification;

	public:
		/// Creates a notification around \a restrictionType.
		explicit TokenRestrictionTypeNotification(model::TokenRestrictionType restrictionType)
				: Notification(Notification_Type, sizeof(TokenRestrictionTypeNotification))
				, RestrictionType(restrictionType)
		{}

	public:
		/// Token restriction type.
		TokenRestrictionType RestrictionType;
	};

	// endregion

	// region TokenRestrictionRequiredNotification

	/// Notification of a required token restriction.
	struct TokenRestrictionRequiredNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = RestrictionToken_Required_Notification;

	public:
		/// Creates a notification around \a tokenId and \a restrictionKey.
		TokenRestrictionRequiredNotification(UnresolvedTokenId tokenId, uint64_t restrictionKey)
				: Notification(Notification_Type, sizeof(TokenRestrictionRequiredNotification))
				, TokenId(tokenId)
				, RestrictionKey(restrictionKey)
		{}

	public:
		/// Identifier of the token being restricted.
		UnresolvedTokenId TokenId;

		/// Restriction key relative to the token id.
		uint64_t RestrictionKey;
	};

	// endregion

	// region TokenGlobalRestrictionModificationNotification

	/// Notification of a token global restriction modification.
	template<NotificationType NotificationType>
	struct TokenGlobalRestrictionModificationNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = NotificationType;

	public:
		/// Creates a notification around \a tokenId, \a referenceTokenId, \a restrictionKey, \a restrictionValue and \a restrictionType.
		TokenGlobalRestrictionModificationNotification(
				UnresolvedTokenId tokenId,
				UnresolvedTokenId referenceTokenId,
				uint64_t restrictionKey,
				uint64_t restrictionValue,
				TokenRestrictionType restrictionType)
				: Notification(Notification_Type, sizeof(TokenGlobalRestrictionModificationNotification))
				, TokenId(tokenId)
				, ReferenceTokenId(referenceTokenId)
				, RestrictionKey(restrictionKey)
				, RestrictionValue(restrictionValue)
				, RestrictionType(restrictionType)
		{}

	public:
		/// Identifier of the token being restricted.
		UnresolvedTokenId TokenId;

		/// Identifier of the token providing the restriction key.
		UnresolvedTokenId ReferenceTokenId;

		/// Restriction key relative to the reference token id.
		uint64_t RestrictionKey;

		/// Restriction value.
		uint64_t RestrictionValue;

		/// Restriction type.
		TokenRestrictionType RestrictionType;
	};

	/// First notification of a token global restriction modification composed of previous values.
	using TokenGlobalRestrictionModificationPreviousValueNotification =
		TokenGlobalRestrictionModificationNotification<RestrictionToken_Global_Restriction_Previous_Notification>;

	/// Second notification of a token global restriction modification composed of new values.
	using TokenGlobalRestrictionModificationNewValueNotification =
		TokenGlobalRestrictionModificationNotification<RestrictionToken_Global_Restriction_New_Notification>;

	// endregion

	// region TokenAddressRestrictionModificationNotification

	/// Notification of a token address restriction modification.
	template<NotificationType NotificationType>
	struct TokenAddressRestrictionModificationNotification : public Notification {
	public:
		/// Matching notification type.
		static constexpr auto Notification_Type = NotificationType;

	public:
		/// Creates a notification around \a tokenId, \a restrictionKey, \a targetAddress and \a restrictionValue.
		TokenAddressRestrictionModificationNotification(
				UnresolvedTokenId tokenId,
				uint64_t restrictionKey,
				const UnresolvedAddress& targetAddress,
				uint64_t restrictionValue)
				: Notification(Notification_Type, sizeof(TokenAddressRestrictionModificationNotification))
				, TokenId(tokenId)
				, RestrictionKey(restrictionKey)
				, TargetAddress(targetAddress)
				, RestrictionValue(restrictionValue)
		{}

	public:
		/// Identifier of the token to which the restriction applies.
		UnresolvedTokenId TokenId;

		/// Restriction key.
		uint64_t RestrictionKey;

		/// Address being restricted.
		const UnresolvedAddress& TargetAddress;

		/// Restriction value.
		uint64_t RestrictionValue;
	};

	/// First notification of a token address restriction modification composed of previous values.
	using TokenAddressRestrictionModificationPreviousValueNotification =
		TokenAddressRestrictionModificationNotification<RestrictionToken_Address_Restriction_Previous_Notification>;

	/// Second notification of a token address restriction modification composed of new values.
	using TokenAddressRestrictionModificationNewValueNotification =
		TokenAddressRestrictionModificationNotification<RestrictionToken_Address_Restriction_New_Notification>;

	// endregion
}}
