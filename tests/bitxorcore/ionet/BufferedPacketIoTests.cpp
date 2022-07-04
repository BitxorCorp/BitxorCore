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

#include "bitxorcore/ionet/BufferedPacketIo.h"
#include "bitxorcore/ionet/PacketSocket.h"
#include "tests/test/net/SocketTestUtils.h"

namespace bitxorcore { namespace ionet {

#define TEST_CLASS BufferedPacketIoTests

	namespace {
		std::shared_ptr<PacketIo> Transform(const std::shared_ptr<PacketSocket>& pSocket) {
			return pSocket->buffered();
		}
	}

	TEST(TEST_CLASS, WriteCanWriteMultipleConsecutivePayloads) {
		test::AssertWriteCanWriteMultipleConsecutivePayloads(Transform);
	}

	TEST(TEST_CLASS, WriteCanWriteMultipleSimultaneousPayloadsWithoutInterleaving) {
		test::AssertWriteCanWriteMultipleSimultaneousPayloadsWithoutInterleaving(Transform);
	}

	TEST(TEST_CLASS, ReadCanReadMultipleConsecutivePayloads) {
		test::AssertReadCanReadMultipleConsecutivePayloads(Transform);
	}

	TEST(TEST_CLASS, ReadCanReadMultipleSimultaneousPayloadsWithoutInterleaving) {
		test::AssertReadCanReadMultipleSimultaneousPayloadsWithoutInterleaving(Transform);
	}
}}
