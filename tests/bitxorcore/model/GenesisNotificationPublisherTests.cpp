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

#include "bitxorcore/model/GenesisNotificationPublisher.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/core/mocks/MockNotificationPublisher.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS GenesisNotificationPublisherTests

	// region ExtractGenesisNotificationPublisherOptions

	TEST(TEST_CLASS, ExtractGenesisNotificationPublisherOptions_CanExtractWhenHarvestNetworkFeesAreDisabled) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		test::FillWithRandomData(config.HarvestNetworkFeeSinkAddress);

		// Act:
		auto options = ExtractGenesisNotificationPublisherOptions(config);

		// Assert:
		EXPECT_EQ(0u, options.SpecialAccountAddresses.size());
	}

	TEST(TEST_CLASS, ExtractGenesisNotificationPublisherOptions_CanExtractWhenHarvestNetworkFeesAreEnabled) {
		// Arrange:
		auto config = BlockchainConfiguration::Uninitialized();
		config.HarvestNetworkPercentage = 15;
		config.ForkHeights.TreasuryReissuance = Height(100);
		test::FillWithRandomData(config.HarvestNetworkFeeSinkAddress);
		test::FillWithRandomData(config.HarvestNetworkFeeSinkAddressPOS);

		// Act:
		auto options = ExtractGenesisNotificationPublisherOptions(config);

		// Assert:
		EXPECT_EQ(1u, options.SpecialAccountAddresses.size());
		EXPECT_EQ(AddressSet{ config.HarvestNetworkFeeSinkAddressPOS }, options.SpecialAccountAddresses);
	}

	// endregion

	// region CreateGenesisNotificationPublisher

	namespace {
		void RunGenesisNotificationPublisherTest(const AddressSet& specialAccountAddresses) {
			// Arrange:
			GenesisNotificationPublisherOptions options;
			options.SpecialAccountAddresses = specialAccountAddresses;

			auto pMockPublisher = std::make_unique<mocks::MockNotificationPublisher>();
			auto pMockPublisherRaw = pMockPublisher.get();
			auto pPublisher = CreateGenesisNotificationPublisher(std::move(pMockPublisher), options);

			auto pMockSubscriber = std::make_unique<mocks::MockNotificationSubscriber>();

			// Act:
			pPublisher->publish(WeakEntityInfo(), *pMockSubscriber);

			// Assert: underlying publisher was called
			EXPECT_EQ(1u, pMockPublisherRaw->numPublishCalls());

			// - one notification was added for each special account
			EXPECT_EQ(specialAccountAddresses.size(), pMockSubscriber->numNotifications());
			EXPECT_EQ(
					std::vector<NotificationType>(specialAccountAddresses.size(), Core_Register_Account_Address_Notification),
					pMockSubscriber->notificationTypes());

			for (const auto& address : specialAccountAddresses)
				EXPECT_TRUE(pMockSubscriber->contains(address));
		}
	}

	TEST(TEST_CLASS, CreateGenesisNotificationPublisher_CanDecorateWithZeroSpecialAccounts) {
		RunGenesisNotificationPublisherTest({});
	}

	TEST(TEST_CLASS, CreateGenesisNotificationPublisher_CanDecorateWithSingleSpecialAccount) {
		RunGenesisNotificationPublisherTest({ test::GenerateRandomByteArray<Address>() });
	}

	TEST(TEST_CLASS, CreateGenesisNotificationPublisher_CanDecorateWithMultipleSpecialAccounts) {
		RunGenesisNotificationPublisherTest({
			test::GenerateRandomByteArray<Address>(),
			test::GenerateRandomByteArray<Address>(),
			test::GenerateRandomByteArray<Address>()
		});
	}

	// endregion
}}
