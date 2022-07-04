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

#include "TokenAddressRestrictionTransactionPlugin.h"
#include "src/model/TokenAddressRestrictionTransaction.h"
#include "src/model/TokenRestrictionNotifications.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		constexpr uint8_t Token_Flags_Restrictable = 0x04;

		template<typename TTransaction>
		void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			// conceptually, token restrictions are token, not account, settings
			// don't raise AddressInteractionNotification between Signer and TargetAddress because token owner always needs to
			// be able to restrict or allow token usage by *any* account

			sub.notify(TokenRequiredNotification(context.SignerAddress, transaction.TokenId, Token_Flags_Restrictable));
			sub.notify(TokenRestrictionRequiredNotification(transaction.TokenId, transaction.RestrictionKey));
			sub.notify(TokenAddressRestrictionModificationPreviousValueNotification(
					transaction.TokenId,
					transaction.RestrictionKey,
					transaction.TargetAddress,
					transaction.PreviousRestrictionValue));
			sub.notify(TokenAddressRestrictionModificationNewValueNotification(
					transaction.TokenId,
					transaction.RestrictionKey,
					transaction.TargetAddress,
					transaction.NewRestrictionValue));
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(TokenAddressRestriction, Default, Publish)
}}
