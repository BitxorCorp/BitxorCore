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

#include "MultisigEntryMapper.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "bitxorcore/utils/Casting.h"

using namespace bitxorcore::mongo::mappers;

namespace bitxorcore { namespace mongo { namespace plugins {

	namespace {
		void StreamAddresses(bson_stream::document& builder, const std::string& name, const state::SortedAddressSet& addresses) {
			auto addressArray = builder << name << bson_stream::open_array;
			for (const auto& address : addresses)
				addressArray << ToBinary(address);

			addressArray << bson_stream::close_array;
		}
	}

	bsoncxx::document::value ToDbModel(const state::MultisigEntry& entry) {
		bson_stream::document builder;
		auto doc = builder
				<< "multisig" << bson_stream::open_document
					<< "version" << 1
					<< "accountAddress" << ToBinary(entry.address())
					<< "minApproval" << static_cast<int32_t>(entry.minApproval())
					<< "minRemoval" << static_cast<int32_t>(entry.minRemoval());

		StreamAddresses(builder, "cosignatoryAddresses", entry.cosignatoryAddresses());
		StreamAddresses(builder, "multisigAddresses", entry.multisigAddresses());

		return doc
				<< bson_stream::close_document
				<< bson_stream::finalize;
	}
}}}
