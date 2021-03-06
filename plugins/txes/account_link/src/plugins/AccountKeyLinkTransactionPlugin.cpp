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

#include "AccountKeyLinkTransactionPlugin.h"
#include "src/model/AccountKeyLinkTransaction.h"
#include "src/model/AccountLinkNotifications.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			if (LinkAction::Link == transaction.LinkAction) {
				// NewRemoteAccountNotification must be raised before AccountPublicKeyNotification because the latter adds account to cache
				sub.notify(NewRemoteAccountNotification(transaction.LinkedPublicKey));
				sub.notify(AccountPublicKeyNotification(transaction.LinkedPublicKey));
			}

			sub.notify(KeyLinkActionNotification(transaction.LinkAction));
			sub.notify(AddressInteractionNotification(context.SignerAddress, transaction.Type, {
				PublicKeyToAddress(transaction.LinkedPublicKey, transaction.Network).template copyTo<UnresolvedAddress>()
			}));
			sub.notify(RemoteAccountKeyLinkNotification(transaction.SignerPublicKey, transaction.LinkedPublicKey, transaction.LinkAction));
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(AccountKeyLink, Default, Publish)
}}
