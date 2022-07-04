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

#include "src/plugins/TokenAliasTransactionPlugin.h"
#include "src/model/AliasNotifications.h"
#include "src/model/TokenAliasTransaction.h"
#include "src/model/NamespaceNotifications.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS TokenAliasTransactionPluginTests

	// region test utils

	namespace {
		DEFINE_TRANSACTION_PLUGIN_WITH_CONFIG_TEST_TRAITS(TokenAlias, NamespaceAliasFeeConfiguration, 1, 1,)
		NamespaceAliasFeeConfiguration CreateRentalFeeConfiguration() {
			UnresolvedTokenId tokeni(1234);
			HeightDependentAddress sinkAddress(test::GenerateRandomByteArray<Address>());
			Key kseys(test::GenerateRandomByteArray<Key>());
			Amount rental(123);

			return { tokeni, sinkAddress, kseys, rental };
		}
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , Entity_Type_Alias_Token, CreateRentalFeeConfiguration())

	// endregion

	// region publish

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrder_Link) {
		// Arrange:
		auto config = CreateRentalFeeConfiguration();
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.AliasAction = AliasAction::Link;

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			NamespaceRequiredNotification::Notification_Type,
			AliasLinkNotification::Notification_Type,
			AliasedTokenIdNotification::Notification_Type,
			TokenRequiredNotification::Notification_Type
		}, config);
	}

	PLUGIN_TEST(CanPublishAllNotifications_Link) {
		// Arrange:
		auto config = CreateRentalFeeConfiguration();
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.AliasAction = AliasAction::Link;

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		builder.template addExpectation<NamespaceRequiredNotification>([&transaction](const auto& notification) {
			EXPECT_TRUE(notification.Owner.isResolved());

			EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
		});
		builder.template addExpectation<AliasLinkNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
			EXPECT_EQ(transaction.AliasAction, notification.AliasAction);
		});
		builder.template addExpectation<AliasedTokenIdNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
			EXPECT_EQ(transaction.AliasAction, notification.AliasAction);
			EXPECT_EQ(transaction.TokenId, notification.AliasedData);
		});
		builder.template addExpectation<TokenRequiredNotification>([&transaction](const auto& notification) {
			EXPECT_TRUE(notification.Owner.isResolved());
			EXPECT_TRUE(notification.TokenId.isResolved());

			EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
			EXPECT_EQ(transaction.TokenId, notification.TokenId.resolved());
			EXPECT_EQ(0u, notification.PropertyFlagMask);
		});

		// Act + Assert:
		builder.runTest(transaction, config);
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrder_Unlink) {
		auto config = CreateRentalFeeConfiguration();
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.AliasAction = AliasAction::Unlink;

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			NamespaceRequiredNotification::Notification_Type,
			AliasLinkNotification::Notification_Type,
			AliasedTokenIdNotification::Notification_Type
		}, config);
	}

	PLUGIN_TEST(CanPublishAllNotifications_Unlink) {
		// Arrange:
		auto config = CreateRentalFeeConfiguration();
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.AliasAction = AliasAction::Unlink;

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		builder.template addExpectation<NamespaceRequiredNotification>([&transaction](const auto& notification) {
			EXPECT_TRUE(notification.Owner.isResolved());

			EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
		});
		builder.template addExpectation<AliasLinkNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
			EXPECT_EQ(transaction.AliasAction, notification.AliasAction);
		});
		builder.template addExpectation<AliasedTokenIdNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.NamespaceId, notification.NamespaceId);
			EXPECT_EQ(transaction.AliasAction, notification.AliasAction);
			EXPECT_EQ(transaction.TokenId, notification.AliasedData);
		});

		// Act + Assert:
		builder.runTest(transaction, config);
	}

	// endregion
}}
