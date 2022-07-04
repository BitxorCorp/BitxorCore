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

#include "TokenDefinitionTransactionPlugin.h"
#include "src/model/TokenDefinitionTransaction.h"
#include "src/model/TokenNotifications.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		auto CreatePublisher(const TokenRentalFeeConfiguration& config) {
			return [config](const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
				// 1. sink account notification
				auto sinkAddress = config.SinkAddress.get(context.BlockHeight);
				sub.notify(AccountAddressNotification(sinkAddress));

				// 2. rental fee charge
				// a. exempt the genesis account
				if (config.GenesisSignerPublicKey != transaction.SignerPublicKey) {
					sub.notify(BalanceTransferNotification(
							context.SignerAddress,
							sinkAddress,
							config.CurrencyTokenId,
							config.Fee,
							BalanceTransferNotification::AmountType::Dynamic));
					sub.notify(TokenRentalFeeNotification(context.SignerAddress, sinkAddress, config.CurrencyTokenId, config.Fee));
				}

				// 3. registration
				auto properties = TokenProperties(transaction.Flags, transaction.Divisibility, transaction.Duration);
				sub.notify(TokenNonceNotification(context.SignerAddress, transaction.Nonce, transaction.Id));
				sub.notify(TokenPropertiesNotification(properties));
				sub.notify(TokenDefinitionNotification(context.SignerAddress, transaction.Id, properties));
			};
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY_WITH_CONFIG(TokenDefinition, Default, CreatePublisher, TokenRentalFeeConfiguration)
}}
