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

#include "MongoReceiptPluginFactory.h"
#include "mappers/MapperUtils.h"

namespace bitxorcore { namespace mongo {

	namespace {
		void StreamBalanceTransferReceipt(bsoncxx::builder::stream::document& builder, const model::BalanceTransferReceipt& receipt) {
			builder
					<< "senderAddress" << mappers::ToBinary(receipt.SenderAddress)
					<< "recipientAddress" << mappers::ToBinary(receipt.RecipientAddress)
					<< "tokenId" << mappers::ToInt64(receipt.Token.TokenId)
					<< "amount" << mappers::ToInt64(receipt.Token.Amount);
		}

		void StreamBalanceChangeReceipt(bsoncxx::builder::stream::document& builder, const model::BalanceChangeReceipt& receipt) {
			builder
					<< "targetAddress" << mappers::ToBinary(receipt.TargetAddress)
					<< "tokenId" << mappers::ToInt64(receipt.Token.TokenId)
					<< "amount" << mappers::ToInt64(receipt.Token.Amount);
		}

		void StreamInflationReceipt(bsoncxx::builder::stream::document& builder, const model::InflationReceipt& receipt) {
			builder
					<< "tokenId" << mappers::ToInt64(receipt.Token.TokenId)
					<< "amount" << mappers::ToInt64(receipt.Token.Amount);
		}
	}

	DEFINE_MONGO_RECEIPT_PLUGIN_FACTORY(BalanceTransfer, StreamBalanceTransferReceipt)
	DEFINE_MONGO_RECEIPT_PLUGIN_FACTORY(BalanceChange, StreamBalanceChangeReceipt)
	DEFINE_MONGO_RECEIPT_PLUGIN_FACTORY(Inflation, StreamInflationReceipt)
}}
