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

#include "TokenAliasTransactionPlugin.h"
#include "src/model/AliasNotifications.h"
#include "src/model/TokenAliasTransaction.h"
#include "src/model/NamespaceNotifications.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {

		template<typename TTransaction>
		static void PublishBalanceTransfer(const NamespaceAliasFeeConfiguration& config, const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			// a. sink account notification
			if (AliasAction::Link == transaction.AliasAction)
			{return;}
			auto sinkAddress = config.SinkAddress.get(context.BlockHeight);
			sub.notify(AccountAddressNotification(sinkAddress));

			auto rentalFee = config.UnlinkRentalFee;
			
			sub.notify(BalanceTransferNotification(
					context.SignerAddress,
					sinkAddress,
					config.CurrencyTokenId,
					rentalFee,
					BalanceTransferNotification::AmountType::Dynamic));
			sub.notify(NamespaceUnlinkAliasFeeNotification(context.SignerAddress, sinkAddress, config.CurrencyTokenId, rentalFee));

		}
		
		template<typename TTransaction>
		auto CreatePublisher(const NamespaceAliasFeeConfiguration& config){
		return [config](const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			
			// 1. rental fee charge
			PublishBalanceTransfer(config, transaction, context, sub);
			sub.notify(NamespaceRequiredNotification(context.SignerAddress, transaction.NamespaceId));
			sub.notify(AliasLinkNotification(transaction.NamespaceId, transaction.AliasAction));
			sub.notify(AliasedTokenIdNotification(transaction.NamespaceId, transaction.AliasAction, transaction.TokenId));

			// in case of unlink, the existence of the (possibly expired) token is guaranteed
			if (AliasAction::Link == transaction.AliasAction)
				sub.notify(TokenRequiredNotification(context.SignerAddress, transaction.TokenId));
		};
		}

	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY_WITH_CONFIG(TokenAlias, Default, CreatePublisher, NamespaceAliasFeeConfiguration)

}}
