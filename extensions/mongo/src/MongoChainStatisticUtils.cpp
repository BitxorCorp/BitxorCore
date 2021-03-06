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

#include "MongoChainStatisticUtils.h"
#include "MongoBulkWriter.h"
#include "bitxorcore/exceptions.h"

using namespace bsoncxx::builder::stream;

namespace bitxorcore { namespace mongo {

	BulkWriteResult TrySetChainStatisticDocument(mongocxx::database& database, const bsoncxx::document::view& upsertDoc) {
		auto collection = database["chainStatistic"];
		auto result = collection.update_one({}, upsertDoc, mongocxx::options::update().upsert(true)).value().result();
		return BulkWriteResult(result);
	}

	bsoncxx::document::value GetChainStatisticDocument(const mongocxx::database& database) {
		auto collection = database["chainStatistic"];
		auto matchedDocument = collection.find_one({});
		if (matchedDocument.has_value())
			return matchedDocument.value();

		return bsoncxx::document::value(nullptr, 0, [](auto*) {});
	}
}}
