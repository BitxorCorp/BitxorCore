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
#include "TokenRestrictionEntry.h"
#include "src/model/TokenRestrictionNotifications.h"
#include "bitxorcore/model/ResolverContext.h"

namespace bitxorcore { namespace state {

	/// Facade around a token global restriction notification used by observers and validators.
	template<model::NotificationType Notification_Type>
	class TokenGlobalRestrictionNotificationFacade {
	public:
		using NotificationType = model::TokenGlobalRestrictionModificationNotification<Notification_Type>;
		using RuleType = TokenGlobalRestriction::RestrictionRule;

	public:
		/// Creates a facade around \a notification and \a resolvers.
		TokenGlobalRestrictionNotificationFacade(const NotificationType& notification, const model::ResolverContext& resolvers)
				: m_notification(notification)
				, m_resolvers(resolvers)
		{}

	public:
		/// Gets the unique key referenced by the notification.
		Hash256 uniqueKey() const {
			return CreateTokenRestrictionEntryKey(m_resolvers.resolve(m_notification.TokenId));
		}

		/// Returns \c true if the notification represents a delete action.
		bool isDeleteAction() const {
			return model::TokenRestrictionType::NONE == m_notification.RestrictionType;
		}

		/// Tries to get the \a rule referenced by the notification from \a entry.
		bool tryGet(const TokenRestrictionEntry& entry, RuleType& rule) const {
			return entry.asGlobalRestriction().tryGet(m_notification.RestrictionKey, rule);
		}

		/// Updates \a entry with the rule referenced by the notification.
		size_t update(TokenRestrictionEntry& entry) const {
			auto& restriction = entry.asGlobalRestriction();
			restriction.set(m_notification.RestrictionKey, toRule());
			return restriction.size();
		}

		/// Returns \c true if the notification indicates an unset value, which is required when adding a new value.
		bool isUnset() const {
			auto notificationRule = toRule();
			return 0 == notificationRule.RestrictionValue && model::TokenRestrictionType::NONE == notificationRule.RestrictionType;
		}

		/// Returns \c true if the notification and \a rule match.
		bool isMatch(const RuleType& rule) const {
			auto notificationRule = toRule();
			return rule.ReferenceTokenId == notificationRule.ReferenceTokenId
					&& rule.RestrictionValue == notificationRule.RestrictionValue
					&& rule.RestrictionType == notificationRule.RestrictionType;
		}

		/// Gets the rule described by the notification.
		RuleType toRule() const {
			return {
				m_resolvers.resolve(m_notification.ReferenceTokenId),
				m_notification.RestrictionValue,
				m_notification.RestrictionType
			};
		}

		/// Gets the restriction described by the notification.
		TokenGlobalRestriction toRestriction() const {
			return TokenGlobalRestriction(m_resolvers.resolve(m_notification.TokenId));
		}

	private:
		const NotificationType& m_notification;
		const model::ResolverContext& m_resolvers;
	};
}}
