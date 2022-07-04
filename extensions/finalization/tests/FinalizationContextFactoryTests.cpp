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

#include "finalization/src/FinalizationContextFactory.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "finalization/tests/test/FinalizationMessageTestUtils.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/local/ServiceLocatorTestContext.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace finalization {

#define TEST_CLASS FinalizationContextFactoryTests

	namespace {
		void AssertCannotCreateFinalizationContext(FinalizationEpoch epoch) {
			// Arrange: setup config
			auto config = finalization::FinalizationConfiguration::Uninitialized();
			config.Size = 9876;
			config.VotingSetGrouping = 50;

			auto blockchainConfig = test::CreatePrototypicalBlockchainConfiguration();
			blockchainConfig.MinVoterBalance = Amount(2'000'000);
			blockchainConfig.VotingSetGrouping = config.VotingSetGrouping;

			// - create a cache
			auto bitxorcoreCache = test::CoreSystemCacheFactory::Create(blockchainConfig);

			test::ServiceTestState testState(std::move(bitxorcoreCache));

			// Act + Assert:
			FinalizationContextFactory factory(config, testState.state());
			EXPECT_THROW(factory.create(epoch), bitxorcore_invalid_argument);
		}

		void AssertCanCreateFinalizationContext(FinalizationEpoch epoch, Height groupedHeight) {
			// Arrange: setup config
			auto config = finalization::FinalizationConfiguration::Uninitialized();
			config.Size = 9876;
			config.VotingSetGrouping = 50;

			auto blockchainConfig = test::CreatePrototypicalBlockchainConfiguration();
			blockchainConfig.MinVoterBalance = Amount(2'000'000);
			blockchainConfig.VotingSetGrouping = config.VotingSetGrouping;

			// - create a cache and seed accounts at groupedHeight (15M) and height (11M)
			auto bitxorcoreCache = test::CoreSystemCacheFactory::Create(blockchainConfig);
			{
				auto bitxorcoreCacheDelta = bitxorcoreCache.createDelta();
				auto& accountStateCacheDelta = bitxorcoreCacheDelta.sub<cache::AccountStateCache>();

				test::AddAccountsWithBalances(accountStateCacheDelta, groupedHeight, blockchainConfig.HarvestingTokenId, {
					Amount(4'000'000), Amount(2'000'000), Amount(1'000'000), Amount(2'000'000), Amount(3'000'000), Amount(4'000'000)
				});
				test::AddAccountsWithBalances(accountStateCacheDelta, groupedHeight + Height(1), blockchainConfig.HarvestingTokenId, {
					Amount(4'000'000), Amount(2'000'000), Amount(1'000'000), Amount(2'000'000), Amount(3'000'000)
				});

				bitxorcoreCache.commit(groupedHeight + Height(1));
			}

			test::ServiceTestState testState(std::move(bitxorcoreCache));
			mocks::SeedStorageWithFixedSizeBlocks(testState.state().storage(), static_cast<uint32_t>(groupedHeight.unwrap()));

			// Act:
			FinalizationContextFactory factory(config, testState.state());
			auto context = factory.create(epoch);

			// Assert: context should be seeded with data from groupedHeight, not height
			auto expectedGenerationHash = testState.state().storage().view().loadBlockElement(groupedHeight)->GenerationHash;

			EXPECT_EQ(epoch, context.epoch());
			EXPECT_EQ(groupedHeight, context.height());
			EXPECT_EQ(expectedGenerationHash, context.generationHash());
			EXPECT_EQ(9876u, context.config().Size);
			EXPECT_EQ(Amount(15'000'000), context.weight());
		}
	}

	TEST(TEST_CLASS, CannotCreateFinalizationContextForEpochLessThanTwo) {
		AssertCannotCreateFinalizationContext(FinalizationEpoch(0));
		AssertCannotCreateFinalizationContext(FinalizationEpoch(1));
	}

	TEST(TEST_CLASS, CanCreateFinalizationContextForEpochTwo) {
		AssertCanCreateFinalizationContext(FinalizationEpoch(2), Height(1));
	}

	TEST(TEST_CLASS, CanCreateFinalizationContextForEpochGreaterThanTwo) {
		AssertCanCreateFinalizationContext(FinalizationEpoch(3), Height(50));
		AssertCanCreateFinalizationContext(FinalizationEpoch(9), Height(350));
	}
}}
