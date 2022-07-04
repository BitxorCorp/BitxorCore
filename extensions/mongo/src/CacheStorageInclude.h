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

#pragma once
#include "ExternalCacheStorage.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include <memory>

namespace bitxorcore { namespace mongo { class MongoStorageContext; } }

/// Declares a mongo cache storage with \a NAME.
#define DECLARE_MONGO_CACHE_STORAGE(NAME) \
	std::unique_ptr<mongo::ExternalCacheStorage> CreateMongo##NAME##CacheStorage( \
			mongo::MongoStorageContext& storageContext, \
			model::NetworkIdentifier networkIdentifier) \

/// Defines a mongo cache storage with \a NAME and \a STORAGE_TYPE using \a TRAITS_NAME.
#define DEFINE_MONGO_CACHE_STORAGE(NAME, STORAGE_TYPE, TRAITS_NAME) \
	DECLARE_MONGO_CACHE_STORAGE(NAME) { \
		return std::make_unique<storages::STORAGE_TYPE<TRAITS_NAME>>(storageContext, networkIdentifier); \
	}

/// Defines a mongo flat cache storage with \a NAME using \a TRAITS_NAME.
#define DEFINE_MONGO_FLAT_CACHE_STORAGE(NAME, TRAITS_NAME) \
	DEFINE_MONGO_CACHE_STORAGE(NAME, MongoFlatCacheStorage, TRAITS_NAME)

/// Defines a mongo historical cache storage with \a NAME using \a TRAITS_NAME.
#define DEFINE_MONGO_HISTORICAL_CACHE_STORAGE(NAME, TRAITS_NAME) \
	DEFINE_MONGO_CACHE_STORAGE(NAME, MongoHistoricalCacheStorage, TRAITS_NAME)
