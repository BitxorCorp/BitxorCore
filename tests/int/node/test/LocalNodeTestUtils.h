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
#include "bitxorcore/ionet/ConnectResult.h"
#include "bitxorcore/ionet/PacketSocket.h"
#include "bitxorcore/local/server/LocalNode.h"
#include "tests/test/core/ThreadPoolTestUtils.h"
#include "tests/test/local/LocalTestUtils.h"
#include "tests/test/genesis/GenesisCompatibleConfiguration.h"
#include "tests/test/net/SocketTestUtils.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	/// Gets the local node port.
	inline unsigned short GetLocalNodePort() {
		return GetLocalHostPort();
	}

	/// Possible node flags.
	enum class NodeFlag {
		/// Node with a single (self) peer.
		Regular = utils::to_underlying_type(LocalNodeFlags::None),

		/// Node with auto harvesting.
		Auto_Harvest = utils::to_underlying_type(LocalNodeFlags::Should_Auto_Harvest),

		/// Node with custom peers.
		Custom_Peers = 4,

		/// Node with a partner node.
		With_Partner = 8,

		/// Simulated api node.
		Simulated_Api = 16,

		/// Node supporting verifiable receipts.
		Verify_Receipts = 32,

		/// Node supporting cache database storage.
		Cache_Database_Storage = 64,

		/// Node supporting verifiable state.
		Verify_State = 128 | Cache_Database_Storage,

		/// Node that should not be booted implicitly.
		Require_Explicit_Boot = 256,

		/// Node supporting auto sync cleanup.
		Auto_Sync_Cleanup = 512,

		/// Bypass seed directory and prepare data directory directly.
		Bypass_Seed = 1024
	};

	MAKE_BITWISE_ENUM(NodeFlag)

	// region counter -> stats adapter

	/// Basic statistics about a local node.
	struct BasicLocalNodeStats {
		/// Number of active packet readers.
		uint64_t NumActiveReaders;

		/// Number of active packet writers.
		uint64_t NumActiveWriters;

		/// Number of scheduled tasks.
		uint64_t NumScheduledTasks;

		/// Number of block elements added to the disruptor.
		uint64_t NumAddedBlockElements;

		/// Number of active block elements in the disruptor.
		uint64_t NumActiveBlockElements;

		/// Number of transaction elements added to the disruptor.
		uint64_t NumAddedTransactionElements;
	};

	/// Statistics about a local p2p node.
	struct PeerLocalNodeStats : public BasicLocalNodeStats {
		/// Number of unlocked accounts.
		uint64_t NumUnlockedAccounts;
	};

	/// Returns \c true if \a counters contains a counter with \a name.
	bool HasCounter(const local::LocalNodeCounterValues& counters, const std::string& name);

	/// Gets the value of the counter with \a name in \a counters.
	uint64_t GetCounterValue(const local::LocalNodeCounterValues& counters, const std::string& name);

	/// Extracts all basic statistics from \a counters.
	BasicLocalNodeStats CountersToBasicLocalNodeStats(const local::LocalNodeCounterValues& counters);

	/// Extracts all basic and peer statistics from \a counters.
	PeerLocalNodeStats CountersToPeerLocalNodeStats(const local::LocalNodeCounterValues& counters);

	// endregion

	// region partner nodes

	/// Creates a local partner node with \a publicKey.
	ionet::Node CreateLocalPartnerNode(const Key& publicKey);

	/// Boots a local partner node around \a config with \a keys and specified \a nodeFlag.
	std::unique_ptr<local::LocalNode> BootLocalPartnerNode(
			config::BitxorCoreConfiguration&& config,
			const config::BitxorCoreKeys& keys,
			NodeFlag nodeFlag);

	/// Prepares bitxorcore configuration (\a config) by  updating setings to be compatible with \a nodeFlag.
	void PrepareBitxorCoreConfiguration(config::BitxorCoreConfiguration& config, NodeFlag nodeFlag);

	/// Prepares bitxorcore configuration (\a config) by adding plugins and extensions (via \a addNodeExtensions)
	/// and updating setings to be compatible with \a nodeFlag.
	template<typename TAddNodeExtensions>
	void PrepareBitxorCoreConfiguration(config::BitxorCoreConfiguration& config, TAddNodeExtensions addNodeExtensions, NodeFlag nodeFlag) {
		PrepareBitxorCoreConfiguration(config, nodeFlag);

		// in order for the genesis block to be processed, at least the transfer plugin needs to be loaded
		AddGenesisPluginExtensions(const_cast<model::BlockchainConfiguration&>(config.Blockchain));
		addNodeExtensions(const_cast<config::ExtensionsConfiguration&>(config.Extensions));
	}

	// endregion

	// region connection tests

	/// Asserts that the local node cannot be connected to on \a port.
	template<typename TTestContext>
	void AssertConnectionError(unsigned short port) {
		// Arrange: boot a local node
		TTestContext context(NodeFlag::Regular);

		// Act: attempt to connect to the node
		auto result = ionet::ConnectResult::Connected;
		std::atomic_bool isConnectionAttemptComplete(false);
		auto pPool = CreateStartedIoThreadPool(1);
		auto options = CreatePacketSocketOptions();
		auto endpoint = CreateLocalHostNodeEndpoint(port);
		auto clientKeyPair = GenerateKeyPair();
		ionet::Connect(pPool->ioContext(), options, endpoint, [&](auto connectResult, const auto&) {
			BITXORCORE_LOG(debug) << "connection attempt completed with " << connectResult;
			result = connectResult;
			isConnectionAttemptComplete = true;
		});
		WAIT_FOR(isConnectionAttemptComplete);

		// Assert: the connection could not be established
		EXPECT_EQ(ionet::ConnectResult::Connect_Error, result);
	}

	/// Represents an external connection.
	struct ExternalConnection {
		/// Connection thread pool.
		std::unique_ptr<thread::IoThreadPool> pPool;

		/// Connection io.
		std::shared_ptr<ionet::PacketIo> pIo;
	};

	/// Creates an external connection to the local node on \a port.
	ExternalConnection CreateExternalConnection(unsigned short port);

	// endregion
}}
