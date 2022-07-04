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
#include "LocalNodeTestContext.h"

namespace bitxorcore { namespace test {

	/// Additional (non-genesis) local node transaction plugins to register.
	enum class NonGenesisTransactionPlugins {
		/// No additional plugins.
		None,

		/// Lock secret plugin.
		Lock_Secret,

		/// Restriction account plugin.
		Restriction_Account
	};

	/// Test context for peer local node tests.
	class PeerLocalNodeTestContext {
	public:
		/// Creates a context around \a nodeFlag, \a additionalPlugins and \a configTransform.
		explicit PeerLocalNodeTestContext(
				NodeFlag nodeFlag = NodeFlag::Regular,
				NonGenesisTransactionPlugins additionalPlugins = NonGenesisTransactionPlugins::None,
				const consumer<config::BitxorCoreConfiguration&>& configTransform = [](const auto&) {});

	public:
		/// Gets the public key of the (primary) local node.
		const Key& publicKey() const;

		/// Gets the (primary) local node.
		local::LocalNode& localNode() const;

		/// Gets the data directory.
		std::string dataDirectory() const;

		/// Gets the node stats.
		PeerLocalNodeStats stats() const;

		/// Gets the current node height via an external connection.
		Height height() const;

		/// Loads saved height from persisted state.
		Height loadSavedStateChainHeight() const;

		/// Creates a copy of the default bitxorcore configuration.
		config::BitxorCoreConfiguration createConfig() const;

	public:
		/// Waits for node height to equal \a height.
		void waitForHeight(Height height) const;

		/// Prepares a fresh data \a directory and returns corresponding configuration.
		config::BitxorCoreConfiguration prepareFreshDataDirectory(const std::string& directory) const;

	public:
		/// Asserts that node has a single reader connection.
		void assertSingleReaderConnection() const;

	public:
		/// Asserts that \a stats indicates a single reader connection.
		static void AssertSingleReaderConnection(const PeerLocalNodeStats& stats);

	private:
		LocalNodeTestContext<LocalNodePeerTraits> m_context;
	};
}}
