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
#include "bitxorcore/ionet/IoTypes.h"
#include "bitxorcore/ionet/PacketSocket.h"
#include "bitxorcore/types.h"
#include <memory>

namespace boost { namespace asio { class io_context; } }

namespace bitxorcore { namespace net { class PacketWriters; } }

namespace bitxorcore { namespace test {

	/// Creates a connection to localhost on \a port configured using \a ioContext.
	std::shared_ptr<ionet::PacketSocket> ConnectToLocalHost(boost::asio::io_context& ioContext, unsigned short port);

	/// Creates a connection to localhost configured with server public key \a serverPublicKey
	/// using \a packetWriters.
	void ConnectToLocalHost(net::PacketWriters& packetWriters, const Key& serverPublicKey);

	/// Starts an async read on \a io that fills \a buffer using \a ioContext.
	void AsyncReadIntoBuffer(boost::asio::io_context& ioContext, ionet::PacketSocket& io, ionet::ByteBuffer& buffer);
}}
