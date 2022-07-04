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
#include "finalization/src/model/FinalizationMessage.h"
#include "finalization/src/model/FinalizationRoundRange.h"
#include "bitxorcore/api/RemoteApi.h"
#include "bitxorcore/model/FinalizationRound.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace ionet { class PacketIo; } }

namespace bitxorcore { namespace api {

	/// Api for retrieving finalization information from a remote node.
	class RemoteFinalizationApi : public RemoteApi {
	protected:
		/// Creates a remote api for the node with specified \a remoteIdentity.
		explicit RemoteFinalizationApi(const model::NodeIdentity& remoteIdentity) : RemoteApi(remoteIdentity)
		{}

	public:
		/// Gets all finalization messages from the remote with a finalization round in \a roundRange
		/// excluding those with hashes in \a knownShortHashes.
		virtual thread::future<model::FinalizationMessageRange> messages(
				const model::FinalizationRoundRange& roundRange,
				model::ShortHashRange&& knownShortHashes) const = 0;
	};

	/// Creates a finalization api for interacting with a remote node with the specified \a io and \a remoteIdentity.
	std::unique_ptr<RemoteFinalizationApi> CreateRemoteFinalizationApi(ionet::PacketIo& io, const model::NodeIdentity& remoteIdentity);
}}
