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

#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/extensions/ServerHooks.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "tests/test/core/PacketPayloadTestUtils.h"
#include "tests/test/core/TransactionInfoTestUtils.h"
#include "tests/test/net/mocks/MockPacketWriters.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS ServiceUtilsTests

	namespace {
		constexpr auto Service_Name = "writers";

		using CosignaturesSink = consumer<const std::vector<model::DetachedCosignature>&>;
		using TransactionSink = extensions::SharedNewTransactionsSink;
	}

	TEST(TEST_CLASS, CanCreatePushEntitySink_BroadcastsNonzeroEntities) {
		// Arrange:
		auto pWriters = std::make_shared<mocks::BroadcastAwareMockPacketWriters>();
		auto transactionInfos = test::CreateTransactionInfos(1);
		auto expectedPayload = ionet::CreateBroadcastPayload(transactionInfos);

		config::BitxorCoreKeys keys;
		ServiceLocator locator(keys);
		locator.registerService(Service_Name, pWriters);

		// Act:
		auto sink = CreatePushEntitySink<TransactionSink>(locator, Service_Name);
		sink(transactionInfos);

		// Assert:
		ASSERT_EQ(1u, pWriters->broadcastedPayloads().size());
		EXPECT_EQ(ionet::PacketType::Push_Transactions, pWriters->broadcastedPayloads()[0].header().Type);
		test::AssertEqualPayload(expectedPayload, pWriters->broadcastedPayloads()[0]);
	}

	TEST(TEST_CLASS, CanCreatePushEntitySink_SkipsBroadcastOfZeroEntities) {
		// Arrange:
		auto pWriters = std::make_shared<mocks::BroadcastAwareMockPacketWriters>();
		auto transactionInfos = test::CreateTransactionInfos(0);

		config::BitxorCoreKeys keys;
		ServiceLocator locator(keys);
		locator.registerService(Service_Name, pWriters);

		// Act:
		auto sink = CreatePushEntitySink<TransactionSink>(locator, Service_Name);
		sink(transactionInfos);

		// Assert:
		ASSERT_EQ(0u, pWriters->broadcastedPayloads().size());
	}

	TEST(TEST_CLASS, CanCreatePushEntitySinkWithCustomPacketType_BroadcastsNonzeroEntities) {
		// Arrange:
		auto pWriters = std::make_shared<mocks::BroadcastAwareMockPacketWriters>();
		auto transactionInfos = test::CreateTransactionInfos(1);
		auto expectedPayload = ionet::CreateBroadcastPayload(transactionInfos, ionet::PacketType::Push_Partial_Transactions);

		config::BitxorCoreKeys keys;
		ServiceLocator locator(keys);
		locator.registerService(Service_Name, pWriters);

		// Act:
		auto sink = CreatePushEntitySink<TransactionSink>(locator, Service_Name, ionet::PacketType::Push_Partial_Transactions);
		sink(transactionInfos);

		// Assert:
		ASSERT_EQ(1u, pWriters->broadcastedPayloads().size());
		EXPECT_EQ(ionet::PacketType::Push_Partial_Transactions, pWriters->broadcastedPayloads()[0].header().Type);
		test::AssertEqualPayload(expectedPayload, pWriters->broadcastedPayloads()[0]);
	}

	TEST(TEST_CLASS, CanCreatePushEntitySinkWithCustomPacketType_SkipsBroadcastOfZeroEntities) {
		// Arrange:
		auto pWriters = std::make_shared<mocks::BroadcastAwareMockPacketWriters>();
		auto transactionInfos = test::CreateTransactionInfos(0);

		config::BitxorCoreKeys keys;
		ServiceLocator locator(keys);
		locator.registerService(Service_Name, pWriters);

		// Act:
		auto sink = CreatePushEntitySink<TransactionSink>(locator, Service_Name, ionet::PacketType::Push_Partial_Transactions);
		sink(transactionInfos);

		// Assert:
		ASSERT_EQ(0u, pWriters->broadcastedPayloads().size());
	}

	TEST(TEST_CLASS, CanCreateCloseConnectionSink) {
		// Arrange:
		auto pWriters = std::make_shared<mocks::BroadcastAwareMockPacketWriters>();
		model::NodeIdentity identity{ test::GenerateRandomByteArray<Key>(), "Alice" };

		// Act:
		auto sink = CreateCloseConnectionSink(*pWriters);
		sink(identity);

		// Assert:
		EXPECT_EQ(1u, pWriters->closedNodeIdentities().size());
		EXPECT_CONTAINS(pWriters->closedNodeIdentities(), identity);
	}
}}
