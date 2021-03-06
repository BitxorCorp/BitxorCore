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

#include "src/plugins/NodeKeyLinkTransactionPlugin.h"
#include "src/model/AccountLinkNotifications.h"
#include "src/model/NodeKeyLinkTransaction.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS NodeKeyLinkTransactionPluginTests

	// region test utils

	namespace {
		DEFINE_TRANSACTION_PLUGIN_TEST_TRAITS(NodeKeyLink, 1, 1,)
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , Entity_Type_Node_Key_Link)

	// endregion

	// region publish - action link

	namespace {
		template<typename TTraits>
		void AddCommonExpectations(
				typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder& builder,
				const typename TTraits::TransactionType& transaction) {
			builder.template addExpectation<KeyLinkActionNotification>([&transaction](const auto& notification) {
				EXPECT_EQ(transaction.LinkAction, notification.LinkAction);
			});
			builder.template addExpectation<NodeKeyLinkNotification>([&transaction](const auto& notification) {
				EXPECT_EQ(transaction.SignerPublicKey, notification.MainAccountPublicKey);
				EXPECT_EQ(transaction.LinkedPublicKey, notification.LinkedPublicKey);
				EXPECT_EQ(transaction.LinkAction, notification.LinkAction);
			});
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenLinkActionIsLink) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.LinkAction = LinkAction::Link;

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			KeyLinkActionNotification::Notification_Type,
			NodeKeyLinkNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenLinkActionIsLink) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.LinkAction = LinkAction::Link;

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion

	// region publish - action unlink

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenLinkActionIsUnlink) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.LinkAction = LinkAction::Unlink;

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			KeyLinkActionNotification::Notification_Type,
			NodeKeyLinkNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenLinkActionIsUnlink) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.LinkAction = LinkAction::Unlink;

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion
}}
