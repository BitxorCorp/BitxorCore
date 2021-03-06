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

#include "SecretLockTransactionPlugin.h"
#include "src/model/SecretLockNotifications.h"
#include "src/model/SecretLockTransaction.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			sub.notify(AccountAddressNotification(transaction.RecipientAddress));
			sub.notify(SecretLockDurationNotification(transaction.Duration));
			sub.notify(SecretLockHashAlgorithmNotification(transaction.HashAlgorithm));
			sub.notify(AddressInteractionNotification(context.SignerAddress, transaction.Type, { transaction.RecipientAddress }));
			sub.notify(BalanceDebitNotification(context.SignerAddress, transaction.Token.TokenId, transaction.Token.Amount));
			sub.notify(SecretLockNotification(
					context.SignerAddress,
					transaction.Token,
					transaction.Duration,
					transaction.HashAlgorithm,
					transaction.Secret,
					transaction.RecipientAddress));

			// raise a zero-transfer notification in order to trigger all token authorization validators
			sub.notify(BalanceTransferNotification(
					context.SignerAddress,
					transaction.RecipientAddress,
					transaction.Token.TokenId,
					Amount(0)));
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(SecretLock, Default, Publish)
}}
