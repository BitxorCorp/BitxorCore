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
#include "partialtransaction/src/PtTypes.h"
#include "bitxorcore/api/RemoteApi.h"
#include "bitxorcore/cache_tx/ShortHashPair.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace ionet { class PacketIo; } }

namespace bitxorcore { namespace api {

	/// Api for retrieving partial transaction information from a remote node.
	class RemotePtApi : public RemoteApi {
	protected:
		/// Creates a remote api for the node with specified \a remoteIdentity.
		explicit RemotePtApi(const model::NodeIdentity& remoteIdentity) : RemoteApi(remoteIdentity)
		{}

	public:
		/// Gets all partial transaction infos from the remote that have a deadline at least \a minDeadline
		/// and do not have a short hash pair in \a knownShortHashPairs.
		virtual thread::future<partialtransaction::CosignedTransactionInfos> transactionInfos(
				Timestamp minDeadline,
				cache::ShortHashPairRange&& knownShortHashPairs) const = 0;
	};

	/// Creates a partial transaction api for interacting with a remote node with the specified \a io and \a remoteIdentity
	/// given transaction \a registry composed of supported transactions.
	std::unique_ptr<RemotePtApi> CreateRemotePtApi(
			ionet::PacketIo& io,
			const model::NodeIdentity& remoteIdentity,
			const model::TransactionRegistry& registry);
}}
