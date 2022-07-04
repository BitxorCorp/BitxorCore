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

#include "TokenAddressRestrictionMapper.h"
#include "mongo/src/MongoTransactionPluginFactory.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/restriction_token/src/model/TokenAddressRestrictionTransaction.h"
#include "plugins/txes/restriction_token/src/model/TokenRestrictionTypes.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	namespace {
		template<typename TTransaction>
		static void Stream(bson_stream::document& builder, const TTransaction& transaction) {
			builder
					<< "tokenId" << ToInt64(transaction.TokenId)
					<< "restrictionKey" << static_cast<int64_t>(transaction.RestrictionKey)
					<< "targetAddress" << ToBinary(transaction.TargetAddress)
					<< "previousRestrictionValue" << static_cast<int64_t>(transaction.PreviousRestrictionValue)
					<< "newRestrictionValue" << static_cast<int64_t>(transaction.NewRestrictionValue);
			}
	}

	DEFINE_MONGO_TRANSACTION_PLUGIN_FACTORY(TokenAddressRestriction, Stream)
}}}
