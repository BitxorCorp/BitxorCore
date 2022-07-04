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

#include "AccountMetadataMapper.h"
#include "TokenMetadataMapper.h"
#include "NamespaceMetadataMapper.h"
#include "storages/MongoMetadataCacheStorage.h"
#include "mongo/src/MongoPluginManager.h"

extern "C" PLUGIN_API
void RegisterMongoSubsystem(bitxorcore::mongo::MongoPluginManager& manager) {
	// transaction support
	manager.addTransactionSupport(bitxorcore::mongo::plugins::CreateAccountMetadataTransactionMongoPlugin());
	manager.addTransactionSupport(bitxorcore::mongo::plugins::CreateTokenMetadataTransactionMongoPlugin());
	manager.addTransactionSupport(bitxorcore::mongo::plugins::CreateNamespaceMetadataTransactionMongoPlugin());

	// cache storage support
	manager.addStorageSupport(bitxorcore::mongo::plugins::CreateMongoMetadataCacheStorage(
			manager.mongoContext(),
			manager.networkIdentifier()));
}
