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

#include "src/plugins/TokenSupplyChangeTransactionPlugin.h"
#include "src/model/TokenNotifications.h"
#include "src/model/TokenSupplyChangeTransaction.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS TokenSupplyChangeTransactionPluginTests

	// region test utils

	namespace {
		DEFINE_TRANSACTION_PLUGIN_TEST_TRAITS(TokenSupplyChange, 1, 1,)
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , Entity_Type_Token_Supply_Change)

	// endregion

	// region publish

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrder) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			TokenRequiredNotification::Notification_Type,
			TokenSupplyChangeNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotifications) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		builder.template addExpectation<TokenRequiredNotification>([&transaction](const auto& notification) {
			EXPECT_TRUE(notification.Owner.isResolved());
			EXPECT_FALSE(notification.TokenId.isResolved());

			EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
			EXPECT_EQ(transaction.TokenId, notification.TokenId.unresolved());
			EXPECT_EQ(0u, notification.PropertyFlagMask);
		});
		builder.template addExpectation<TokenSupplyChangeNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(GetSignerAddress(transaction), notification.Owner);
			EXPECT_EQ(transaction.TokenId, notification.TokenId);
			EXPECT_EQ(transaction.Action, notification.Action);
			EXPECT_EQ(transaction.Delta, notification.Delta);
		});

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion
}}
