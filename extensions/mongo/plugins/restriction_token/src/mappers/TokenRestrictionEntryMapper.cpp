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

#include "TokenRestrictionEntryMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "bitxorcore/utils/Casting.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	// region ToDbModel

	namespace {
		void StreamAddressRestriction(bson_stream::document& builder, const state::TokenAddressRestriction& addressRestriction) {
			builder
					<< "tokenId" << ToInt64(addressRestriction.tokenId())
					<< "targetAddress" << ToBinary(addressRestriction.address());

			auto restrictionArray = builder << "restrictions" << bson_stream::open_array;
			for (auto key : addressRestriction.keys()) {
				restrictionArray
						<< bson_stream::open_document
							<< "key" << static_cast<int64_t>(key)
							<< "value" << static_cast<int64_t>(addressRestriction.get(key))
						<< bson_stream::close_document;
			}

			restrictionArray << bson_stream::close_array;
		}

		void StreamGlobalRestriction(bson_stream::document& builder, const state::TokenGlobalRestriction& globalRestriction) {
			builder << "tokenId" << ToInt64(globalRestriction.tokenId());

			auto restrictionArray = builder << "restrictions" << bson_stream::open_array;
			for (auto key : globalRestriction.keys()) {
				state::TokenGlobalRestriction::RestrictionRule rule;
				globalRestriction.tryGet(key, rule);
				restrictionArray
						<< bson_stream::open_document
							<< "key" << static_cast<int64_t>(key)
							<< "restriction" << bson_stream::open_document
								<< "referenceTokenId" << ToInt64(rule.ReferenceTokenId)
								<< "restrictionValue" << static_cast<int64_t>(rule.RestrictionValue)
								<< "restrictionType" << utils::to_underlying_type(rule.RestrictionType)
							<< bson_stream::close_document
						<< bson_stream::close_document;
			}

			restrictionArray << bson_stream::close_array;
		}
	}

	bsoncxx::document::value ToDbModel(const state::TokenRestrictionEntry& restrictionEntry) {
		bson_stream::document builder;
		auto doc = builder
				<< "tokenRestrictionEntry" << bson_stream::open_document
					<< "version" << 1
					<< "compositeHash" << ToBinary(restrictionEntry.uniqueKey())
					<< "entryType" << utils::to_underlying_type(restrictionEntry.entryType());

		if (state::TokenRestrictionEntry::EntryType::Address == restrictionEntry.entryType())
			StreamAddressRestriction(builder, restrictionEntry.asAddressRestriction());
		else
			StreamGlobalRestriction(builder, restrictionEntry.asGlobalRestriction());

		return doc
				<< bson_stream::close_document
				<< bson_stream::finalize;
	}

	// endregion
}}}
