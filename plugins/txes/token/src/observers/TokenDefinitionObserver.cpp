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

#include "Observers.h"
#include "src/cache/TokenCache.h"

namespace bitxorcore { namespace observers {

	namespace {
		model::TokenFlags Xor(model::TokenFlags lhs, model::TokenFlags rhs) {
			return static_cast<model::TokenFlags>(utils::to_underlying_type(lhs) ^ utils::to_underlying_type(rhs));
		}

		model::TokenProperties MergeProperties(
				const model::TokenProperties& currentProperties,
				const model::TokenProperties& notificationProperties,
				NotifyMode mode) {
			auto flags = Xor(currentProperties.flags(), notificationProperties.flags());
			auto divisibility = static_cast<uint8_t>(currentProperties.divisibility() ^ notificationProperties.divisibility());
			auto duration = NotifyMode::Commit == mode
					? currentProperties.duration() + notificationProperties.duration()
					: currentProperties.duration() - notificationProperties.duration();
			return model::TokenProperties(flags, divisibility, duration);
		}

		auto ApplyNotification(
				state::TokenEntry& currentTokenEntry,
				const model::TokenDefinitionNotification& notification,
				NotifyMode mode) {
			const auto& currentDefinition = currentTokenEntry.definition();
			auto newProperties = MergeProperties(currentDefinition.properties(), notification.Properties, mode);
			auto revision = NotifyMode::Commit == mode ? currentDefinition.revision() + 1 : currentDefinition.revision() - 1;
			auto definition = state::TokenDefinition(currentDefinition.startHeight(), notification.Owner, revision, newProperties);
			return state::TokenEntry(notification.TokenId, definition);
		}
	}

	DEFINE_OBSERVER(TokenDefinition, model::TokenDefinitionNotification, [](
			const model::TokenDefinitionNotification& notification,
			const ObserverContext& context) {
		auto& tokenCache = context.Cache.sub<cache::TokenCache>();

		// token supply will always be zero when a token definition is observed
		auto tokenIter = tokenCache.find(notification.TokenId);
		if (tokenIter.tryGet()) {
			// copy existing token entry before removing
			auto tokenEntry = tokenIter.get();
			tokenCache.remove(notification.TokenId);

			if (NotifyMode::Rollback == context.Mode && 1 == tokenEntry.definition().revision())
				return;

			tokenCache.insert(ApplyNotification(tokenEntry, notification, context.Mode));
		} else {
			auto definition = state::TokenDefinition(context.Height, notification.Owner, 1, notification.Properties);
			tokenCache.insert(state::TokenEntry(notification.TokenId, definition));
		}
	})
}}
