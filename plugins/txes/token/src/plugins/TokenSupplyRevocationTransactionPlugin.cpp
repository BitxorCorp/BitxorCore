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

#include "TokenSupplyRevocationTransactionPlugin.h"
#include "src/model/TokenFlags.h"
#include "src/model/TokenSupplyRevocationTransaction.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		auto CreatePublisher(const Address& genesisAddress) {
			return [genesisAddress](const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
				auto isGenesisSigner = genesisAddress == context.SignerAddress;
				auto requiredTokenFlags = utils::to_underlying_type(isGenesisSigner ? TokenFlags::None : TokenFlags::Revokable);

				// TokenFlagsValidator prevents any tokens from being created with Revokable flag prior to fork block
				// consequently, TokenSupplyRevocation transactions will be rejected until then because of Revokable flag requirement
				sub.notify(TokenRequiredNotification(context.SignerAddress, transaction.Token.TokenId, requiredTokenFlags));

				sub.notify(BalanceTransferNotification(
						transaction.SourceAddress,
						context.SignerAddress,
						transaction.Token.TokenId,
						transaction.Token.Amount));
			};
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY_WITH_CONFIG(TokenSupplyRevocation, Default, CreatePublisher, Address)
}}
