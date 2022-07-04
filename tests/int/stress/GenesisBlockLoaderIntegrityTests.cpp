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

#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/extensions/GenesisBlockLoader.h"
#include "tests/test/local/LocalNodeTestState.h"
#include "tests/test/local/LocalTestUtils.h"
#include "tests/test/genesis/GenesisCompatibleConfiguration.h"
#include "tests/test/genesis/GenesisTestUtils.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/test/nodeps/TestNetworkConstants.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS GenesisBlockLoaderIntegrityTests

	namespace {
		// region test utils

		// Num_Genesis_Transactions - genesis block has:
		// 1) Num_Genesis_Namespaces namespace registration transactions
		// 2) for each token - (a) token definition transaction, (b) token supply change transaction, (c) token alias transaction
		// 3) Num_Genesis_Accounts transfer transactions
		// 4) Num_Genesis_Harvesting_Accounts vrf key link transactions

		constexpr auto Num_Genesis_Accounts = CountOf(test::Test_Network_Private_Keys);
		constexpr auto Num_Genesis_Harvesting_Accounts = CountOf(test::Test_Network_Vrf_Private_Keys);
		constexpr auto Num_Genesis_Namespaces = 3;
		constexpr auto Num_Genesis_Tokens = 2;
		constexpr auto Num_Genesis_Transactions = 0
				+ Num_Genesis_Namespaces
				+ 3 * Num_Genesis_Tokens
				+ Num_Genesis_Accounts
				+ Num_Genesis_Harvesting_Accounts;

		template<typename TAction>
		void RunGenesisBlockTest(TAction action) {
			// Arrange:
			test::TempDirectoryGuard tempDir;
			config::BitxorCoreDataDirectoryPreparer::Prepare(tempDir.name());

			auto config = test::CreatePrototypicalBitxorCoreConfiguration(tempDir.name());
			test::AddGenesisPluginExtensions(const_cast<model::BlockchainConfiguration&>(config.Blockchain));

			auto pPluginManager = test::CreatePluginManagerWithRealPlugins(config);
			test::LocalNodeTestState localNodeState(pPluginManager->config(), tempDir.name(), pPluginManager->createCache());

			{
				auto cacheDelta = localNodeState.ref().Cache.createDelta();

				// Act:
				GenesisBlockLoader loader(cacheDelta, *pPluginManager, pPluginManager->createObserver());
				loader.executeAndCommit(localNodeState.ref(), StateHashVerification::Enabled);
			}

			// Assert:
			action(localNodeState.ref());
		}

		// endregion
	}

	// region tests

	TEST(TEST_CLASS, ProperAccountStateAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert:
			const auto& cacheView = stateRef.Cache.createView();
			EXPECT_EQ(Height(1), cacheView.height());
			test::AssertGenesisAccountState(cacheView);
		});
	}

	TEST(TEST_CLASS, ProperTokenStateAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert:
			const auto& cacheView = stateRef.Cache.createView();
			test::AssertGenesisNamespaceState(cacheView);
			test::AssertGenesisTokenState(cacheView);
		});
	}

	TEST(TEST_CLASS, ProperChainScoreAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert: GenesisBlockLoader doesn't modify score
			EXPECT_EQ(model::ChainScore(0), stateRef.Score.get());
		});
	}

	TEST(TEST_CLASS, ProperRecalculationHeightAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert:
			EXPECT_EQ(model::ImportanceHeight(1), stateRef.Cache.createView().dependentState().LastRecalculationHeight);
		});
	}

	TEST(TEST_CLASS, ProperFinalizedHeightAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert:
			EXPECT_EQ(Height(1), stateRef.Cache.createView().dependentState().LastFinalizedHeight);
		});
	}

	TEST(TEST_CLASS, ProperNumTransactionsAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			// Assert:
			EXPECT_EQ(Num_Genesis_Transactions, stateRef.Cache.createView().dependentState().NumTotalTransactions);
		});
	}

	TEST(TEST_CLASS, ProperHighValueAccountStatisticsAfterLoadingGenesisBlock) {
		// Act:
		RunGenesisBlockTest([](const auto& stateRef) {
			auto cacheView = stateRef.Cache.createView();
			auto readOnlyCache = cacheView.toReadOnly();
			const auto& accountStateCache = readOnlyCache.template sub<cache::AccountStateCache>();
			auto highValueAccountStatistics = accountStateCache.highValueAccountStatistics(FinalizationEpoch(0));

			// Assert:
			EXPECT_EQ(0u, highValueAccountStatistics.VotingEligibleAccountsCount);
			EXPECT_EQ(Num_Genesis_Harvesting_Accounts, highValueAccountStatistics.HarvestingEligibleAccountsCount);
			EXPECT_EQ(Amount(), highValueAccountStatistics.TotalVotingBalance);
		});
	}

	// endregion
}}
