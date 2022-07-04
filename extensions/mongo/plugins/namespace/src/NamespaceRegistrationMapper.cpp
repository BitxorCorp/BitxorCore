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

#include "NamespaceRegistrationMapper.h"
#include "mongo/src/MongoTransactionPluginFactory.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/namespace/src/model/NamespaceRegistrationTransaction.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	namespace {
		template<typename TTransaction>
		void StreamTransaction(bson_stream::document& builder, const TTransaction& transaction) {
			if (0 == transaction.NameSize)
				BITXORCORE_THROW_RUNTIME_ERROR("cannot map namespace registration transaction without name");

			builder << "registrationType" << utils::to_underlying_type(transaction.RegistrationType);

			if (transaction.IsRootRegistration())
				builder << "duration" << ToInt64(transaction.Duration);
			else
				builder << "parentId" << ToInt64(transaction.ParentId);

			builder
					<< "id" << ToInt64(transaction.Id)
					<< "name" << ToBinary(transaction.NamePtr(), transaction.NameSize);
		}
	}

	DEFINE_MONGO_TRANSACTION_PLUGIN_FACTORY(NamespaceRegistration, StreamTransaction)
}}}
