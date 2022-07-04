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
#include "bitxorcore/ionet/Packet.h"
#include "bitxorcore/ionet/PacketHandlers.h"

namespace bitxorcore { namespace test {

	/// Asserts that the specified \a payload is a thin wrapper around the specified \a packet.
	void AssertPacketPayload(const ionet::Packet& packet, const ionet::PacketPayload& payload);

	/// Asserts that \a payload is unset.
	void AssertPacketPayloadUnset(const ionet::PacketPayload& payload);

	/// Asserts that \a payload has a header with size (\a expectedSize) and type (\a expectedType).
	void AssertPacketHeader(const ionet::PacketPayload& payload, size_t expectedSize, ionet::PacketType expectedType);

	/// Asserts that \a handlerContext does not have a response.
	void AssertNoResponse(const ionet::ServerPacketHandlerContext& handlerContext);

	/// Asserts that \a handlerContext has a response payload header with size (\a expectedSize) and type (\a expectedType).
	void AssertPacketHeader(const ionet::ServerPacketHandlerContext& handlerContext, size_t expectedSize, ionet::PacketType expectedType);

	/// Asserts that \a handlerContext has a response with a single buffer and returns the buffer.
	const uint8_t* GetSingleBufferData(const ionet::ServerPacketHandlerContext& handlerContext);

	/// Asserts that \a expectedPayload is equal to \a payload.
	void AssertEqualPayload(const ionet::PacketPayload& expectedPayload, const ionet::PacketPayload& payload);
}}
