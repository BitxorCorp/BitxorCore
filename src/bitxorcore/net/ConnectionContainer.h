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
#include "bitxorcore/model/NodeIdentity.h"
#include "bitxorcore/functions.h"

namespace bitxorcore {
	namespace ionet { class PacketSocketInfo; }
	namespace net { struct PeerConnectResult; }
}

namespace bitxorcore { namespace net {

	/// Manages a collection of connections.
	class ConnectionContainer {
	public:
		virtual ~ConnectionContainer() = default;

	public:
		/// Gets the number of active connections (including pending connections).
		virtual size_t numActiveConnections() const = 0;

		/// Gets the identities of active connections.
		virtual model::NodeIdentitySet identities() const = 0;

	public:
		/// Closes any active connections to the node identified by \a identity.
		virtual bool closeOne(const model::NodeIdentity& identity) = 0;
	};

	/// Manages a collection of accepted connections.
	class AcceptedConnectionContainer : public ConnectionContainer {
	public:
		using AcceptCallback = predicate<const PeerConnectResult&>;

	public:
		/// Accepts a connection represented by \a socketInfo and calls \a callback on completion.
		virtual void accept(const ionet::PacketSocketInfo& socketInfo, const AcceptCallback& callback) = 0;
	};
}}
