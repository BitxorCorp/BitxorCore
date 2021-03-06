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
#include "mongo/src/mappers/MapperInclude.h"
#include "mongo/src/mappers/MapperUtils.h"
#include "plugins/txes/lock_shared/src/state/LockInfo.h"
#include "bitxorcore/utils/Casting.h"

namespace bitxorcore { namespace mongo { namespace plugins {

	/// Traits based lock info mapper.
	template<typename TTraits>
	class LockInfoMapper {
	private:
		using LockInfoType = typename TTraits::LockInfoType;

		// region ToDbModel

	private:
		static void StreamLockInfo(mappers::bson_stream::document& builder, const state::LockInfo& lockInfo) {
			using namespace bitxorcore::mongo::mappers;

			builder
					<< "version" << 1
					<< "ownerAddress" << ToBinary(lockInfo.OwnerAddress)
					<< "tokenId" << ToInt64(lockInfo.TokenId)
					<< "amount" << ToInt64(lockInfo.Amount)
					<< "endHeight" << ToInt64(lockInfo.EndHeight)
					<< "status" << utils::to_underlying_type(lockInfo.Status);
		}

	public:
		/// Maps \a lockInfo to the corresponding db model value.
		static bsoncxx::document::value ToDbModel(const LockInfoType& lockInfo) {
			mappers::bson_stream::document builder;
			auto doc = builder << "lock" << mappers::bson_stream::open_document;
			StreamLockInfo(builder, lockInfo);
			TTraits::StreamLockInfo(builder, lockInfo);
			return doc
					<< mappers::bson_stream::close_document
					<< mappers::bson_stream::finalize;
		}

		// endregion
	};
}}}
