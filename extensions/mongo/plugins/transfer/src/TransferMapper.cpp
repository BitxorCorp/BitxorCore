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

#include "TransferMapper.h"
#include "mongo/src/MongoTransactionPluginFactory.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/transfer/src/model/TransferTransaction.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	namespace {
		void StreamMessage(bson_stream::document& builder, const uint8_t* pMessage, size_t messageSize) {
			if (0 == messageSize)
				return;

			builder << "message" << ToBinary(pMessage, messageSize);
		}

		void StreamTokens(bson_stream::document& builder, const model::UnresolvedToken* pToken, size_t numTokens) {
			auto tokensArray = builder << "tokens" << bson_stream::open_array;
			for (auto i = 0u; i < numTokens; ++i) {
				StreamToken(tokensArray, pToken->TokenId, pToken->Amount);
				++pToken;
			}

			tokensArray << bson_stream::close_array;
		}

		template<typename TTransaction>
		void StreamTransaction(bson_stream::document& builder, const TTransaction& transaction) {
			builder << "recipientAddress" << ToBinary(transaction.RecipientAddress);
			StreamMessage(builder, transaction.MessagePtr(), transaction.MessageSize);
			StreamTokens(builder, transaction.TokensPtr(), transaction.TokensCount);
		}
	}

	DEFINE_MONGO_TRANSACTION_PLUGIN_FACTORY(Transfer, StreamTransaction)
}}}
