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

#include "TokenEntryMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/token/src/state/TokenEntry.h"
#include "bitxorcore/utils/Casting.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	// region ToDbModel

	namespace {
		void StreamProperties(bson_stream::document& builder, const model::TokenProperties& properties) {
			builder
					<< "flags" << utils::to_underlying_type(properties.flags())
					<< "divisibility" << properties.divisibility()
					<< "duration" << ToInt64(properties.duration());
		}
	}

	bsoncxx::document::value ToDbModel(const state::TokenEntry& tokenEntry) {
		const auto& definition = tokenEntry.definition();
		bson_stream::document builder;
		auto doc = builder
				<< "token" << bson_stream::open_document
					<< "version" << 1
					<< "id" << ToInt64(tokenEntry.tokenId())
					<< "supply" << ToInt64(tokenEntry.supply())
					<< "startHeight" << ToInt64(definition.startHeight())
					<< "ownerAddress" << ToBinary(definition.ownerAddress())
					<< "revision" << static_cast<int32_t>(definition.revision());

		StreamProperties(builder, definition.properties());

		doc << bson_stream::close_document;

		return builder << bson_stream::finalize;
	}

	// endregion
}}}
