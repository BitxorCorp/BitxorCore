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

#include "TokenDefinitionMapper.h"
#include "TokenSupplyChangeMapper.h"
#include "TokenSupplyRevocationMapper.h"
#include "storages/MongoTokenCacheStorage.h"
#include "mongo/src/MongoPluginManager.h"
#include "mongo/src/MongoReceiptPluginFactory.h"
#include "plugins/txes/token/src/model/TokenReceiptType.h"

extern "C" PLUGIN_API
void RegisterMongoSubsystem(bitxorcore::mongo::MongoPluginManager& manager) {
	using namespace bitxorcore;

	// transaction support
	manager.addTransactionSupport(mongo::plugins::CreateTokenDefinitionTransactionMongoPlugin());
	manager.addTransactionSupport(mongo::plugins::CreateTokenSupplyChangeTransactionMongoPlugin());
	manager.addTransactionSupport(mongo::plugins::CreateTokenSupplyRevocationTransactionMongoPlugin());

	// cache storage support
	manager.addStorageSupport(mongo::plugins::CreateMongoTokenCacheStorage(manager.mongoContext(), manager.networkIdentifier()));

	// receipt support
	manager.addReceiptSupport(mongo::CreateArtifactExpiryReceiptMongoPlugin<TokenId>(model::Receipt_Type_Token_Expired));
	manager.addReceiptSupport(mongo::CreateBalanceTransferReceiptMongoPlugin(model::Receipt_Type_Token_Rental_Fee));
}
