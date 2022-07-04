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

#include "src/plugins/TokenGlobalRestrictionTransactionPlugin.h"
#include "sdk/src/extensions/ConversionExtensions.h"
#include "src/model/TokenGlobalRestrictionTransaction.h"
#include "src/model/TokenRestrictionNotifications.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS TokenGlobalRestrictionTransactionPluginTests

	// region test utils

	namespace {
		DEFINE_TRANSACTION_PLUGIN_TEST_TRAITS(TokenGlobalRestriction, 1, 1,)
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , Entity_Type_Token_Global_Restriction)

	// endregion

	// region publish - with reference token id

	namespace {
		template<typename TTraits>
		void AddCommonExpectations(
				typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder& builder,
				const typename TTraits::TransactionType& transaction) {
			builder.template addExpectation<TokenRestrictionTypeNotification>([&transaction](const auto& notification) {
				EXPECT_EQ(transaction.NewRestrictionType, notification.RestrictionType);
			});
			builder.template addExpectation<TokenRequiredNotification>([&transaction](const auto& notification) {
				EXPECT_TRUE(notification.Owner.isResolved());
				EXPECT_FALSE(notification.TokenId.isResolved());

				EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
				EXPECT_EQ(transaction.TokenId, notification.TokenId.unresolved());
				EXPECT_EQ(0x04u, notification.PropertyFlagMask);
			});
			builder.template addExpectation<TokenGlobalRestrictionModificationPreviousValueNotification>([&transaction](
					const auto& notification) {
				EXPECT_EQ(transaction.TokenId, notification.TokenId);
				EXPECT_EQ(transaction.ReferenceTokenId, notification.ReferenceTokenId);
				EXPECT_EQ(transaction.RestrictionKey, notification.RestrictionKey);
				EXPECT_EQ(transaction.PreviousRestrictionValue, notification.RestrictionValue);
				EXPECT_EQ(transaction.PreviousRestrictionType, notification.RestrictionType);
			});
			builder.template addExpectation<TokenGlobalRestrictionModificationNewValueNotification>([&transaction](
					const auto& notification) {
				EXPECT_EQ(transaction.TokenId, notification.TokenId);
				EXPECT_EQ(transaction.ReferenceTokenId, notification.ReferenceTokenId);
				EXPECT_EQ(transaction.RestrictionKey, notification.RestrictionKey);
				EXPECT_EQ(transaction.NewRestrictionValue, notification.RestrictionValue);
				EXPECT_EQ(transaction.NewRestrictionType, notification.RestrictionType);
			});
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenReferenceTokenIdIsProvided) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.ReferenceTokenId = UnresolvedTokenId(test::Random() | 1);

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			TokenRestrictionTypeNotification::Notification_Type,
			TokenRequiredNotification::Notification_Type,
			TokenRestrictionRequiredNotification::Notification_Type,
			TokenGlobalRestrictionModificationPreviousValueNotification::Notification_Type,
			TokenGlobalRestrictionModificationNewValueNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenReferenceTokenIdIsProvided) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.ReferenceTokenId = UnresolvedTokenId(test::Random() | 1);

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);
		builder.template addExpectation<TokenRestrictionRequiredNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.ReferenceTokenId, notification.TokenId);
			EXPECT_EQ(transaction.RestrictionKey, notification.RestrictionKey);
		});

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion

	// region publish - without reference token id

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenReferenceTokenIdIsNotProvided) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.ReferenceTokenId = UnresolvedTokenId();

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			TokenRestrictionTypeNotification::Notification_Type,
			TokenRequiredNotification::Notification_Type,
			TokenGlobalRestrictionModificationPreviousValueNotification::Notification_Type,
			TokenGlobalRestrictionModificationNewValueNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenReferenceTokenIdIsNotProvided) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.ReferenceTokenId = UnresolvedTokenId();

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion
}}
