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
#include "bitxorcore/ionet/NodeInteractionResult.h"
#include "bitxorcore/ionet/NodeSet.h"
#include "bitxorcore/net/PacketIoPickerContainer.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace nodediscovery {

	/// Creates a batch peers requestor.
	class BatchPeersRequestor {
	private:
		using NodesConsumer = consumer<const ionet::NodeSet&>;
		using RemoteApiResults = std::vector<ionet::NodeInteractionResult>;

	public:
		/// Creates a requestor around \a packetIoPickers, which is used to find partners. Forwards found nodes to \a nodesConsumer.
		BatchPeersRequestor(const net::PacketIoPickerContainer& packetIoPickers, const NodesConsumer& nodesConsumer);

	public:
		/// Finds and forwards peers of peers within the specified \a timeout.
		thread::future<RemoteApiResults> findPeersOfPeers(const utils::TimeSpan& timeout) const;

	private:
		net::PacketIoPickerContainer m_packetIoPickers; // held by value because packet io pickers is tied to ServiceState
		NodesConsumer m_nodesConsumer;
	};
}}
