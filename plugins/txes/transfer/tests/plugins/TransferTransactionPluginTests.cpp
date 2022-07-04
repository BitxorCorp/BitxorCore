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

#include "src/plugins/TransferTransactionPlugin.h"
#include "sdk/src/extensions/ConversionExtensions.h"
#include "src/model/TransferNotifications.h"
#include "src/model/TransferTransaction.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS TransferTransactionPluginTests

	// region test utils

	namespace {
		DEFINE_TRANSACTION_PLUGIN_TEST_TRAITS(Transfer, 1, 1,)

		template<typename TTraits>
		auto CreateTransactionWithTokens(uint8_t numTokens, uint16_t messageSize = 0) {
			using TransactionType = typename TTraits::TransactionType;
			uint32_t entitySize = SizeOf32<TransactionType>() + numTokens * SizeOf32<UnresolvedToken>() + messageSize;
			auto pTransaction = utils::MakeUniqueWithSize<TransactionType>(entitySize);
			test::FillWithRandomData({ reinterpret_cast<uint8_t*>(pTransaction.get()), entitySize });

			pTransaction->Size = entitySize;
			pTransaction->MessageSize = messageSize;
			pTransaction->TokensCount = numTokens;
			return pTransaction;
		}
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, , , Entity_Type_Transfer)

	// endregion

	// region publish - neither message nor tokens

	namespace {
		template<typename TTraits>
		void AddCommonExpectations(
				typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder& builder,
				const typename TTraits::TransactionType& transaction) {
			builder.template addExpectation<InternalPaddingNotification>([&transaction](const auto& notification) {
				auto expectedPadding = transaction.TransferTransactionBody_Reserved1 << 8 | transaction.TransferTransactionBody_Reserved2;
				EXPECT_EQ(expectedPadding, notification.Padding);
			});
			builder.template addExpectation<AccountAddressNotification>([&transaction](const auto& notification) {
				EXPECT_FALSE(notification.Address.isResolved());

				EXPECT_EQ(transaction.RecipientAddress, notification.Address.unresolved());
			});
			builder.template addExpectation<AddressInteractionNotification>([&transaction](const auto& notification) {
				EXPECT_EQ(GetSignerAddress(transaction), notification.Source);
				EXPECT_EQ(transaction.Type, notification.TransactionType);
				EXPECT_EQ(UnresolvedAddressSet{ transaction.RecipientAddress }, notification.ParticipantsByAddress);
			});
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenNeitherMessageNorTokensArePresent) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.MessageSize = 0;
		transaction.TokensCount = 0;

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
			InternalPaddingNotification::Notification_Type,
			AccountAddressNotification::Notification_Type,
			AddressInteractionNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenNeitherMessageNorTokensArePresent) {
		// Arrange:
		typename TTraits::TransactionType transaction;
		test::FillWithRandomData(transaction);
		transaction.MessageSize = 0;
		transaction.TokensCount = 0;

		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion

	// region publish - message only

	namespace {
		template<typename TTransaction>
		void PrepareMessageOnlyTransaction(TTransaction& transaction) {
			test::FillWithRandomData(transaction);
			transaction.TokensCount = 0;
			transaction.MessageSize = 17;
			transaction.Size = static_cast<uint32_t>(TTransaction::CalculateRealSize(transaction));
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenMessageOnlyIsPresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(0, 17);
		PrepareMessageOnlyTransaction(*pTransaction);

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(*pTransaction, {
			InternalPaddingNotification::Notification_Type,
			AccountAddressNotification::Notification_Type,
			AddressInteractionNotification::Notification_Type,
			TransferMessageNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenMessageOnlyIsPresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(0, 17);
		PrepareMessageOnlyTransaction(*pTransaction);

		const auto& transaction = *pTransaction;
		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);
		builder.template addExpectation<TransferMessageNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.SignerPublicKey, notification.SenderPublicKey);
			EXPECT_EQ(transaction.RecipientAddress, notification.Recipient);
			EXPECT_EQ(17u, notification.MessageSize);
			EXPECT_EQ(transaction.MessagePtr(), notification.MessagePtr);
		});

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion

	// region publish - tokens only

	namespace {
		template<typename TTransaction>
		void PrepareTokensOnlyTransaction(TTransaction& transaction) {
			test::FillWithRandomData(transaction);
			transaction.TokensCount = 2;
			transaction.MessageSize = 0;
			transaction.Size = static_cast<uint32_t>(TTransaction::CalculateRealSize(transaction));
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenTokensOnlyArePresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(2, 0);
		PrepareTokensOnlyTransaction(*pTransaction);

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(*pTransaction, {
			InternalPaddingNotification::Notification_Type,
			AccountAddressNotification::Notification_Type,
			AddressInteractionNotification::Notification_Type,
			BalanceTransferNotification::Notification_Type,
			BalanceTransferNotification::Notification_Type,
			TransferTokensNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenTokensOnlyArePresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(2, 0);
		PrepareTokensOnlyTransaction(*pTransaction);

		const auto& transaction = *pTransaction;
		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);
		for (auto i = 0u; i < 2u; ++i) {
			builder.template addExpectation<BalanceTransferNotification>(i, [&transaction, i](const auto& notification) {
				EXPECT_TRUE(notification.Sender.isResolved());
				EXPECT_FALSE(notification.Recipient.isResolved());

				EXPECT_EQ(GetSignerAddress(transaction), notification.Sender.resolved());
				EXPECT_EQ(transaction.TokensPtr()[i].TokenId, notification.TokenId);
				EXPECT_EQ(transaction.TokensPtr()[i].Amount, notification.Amount);
				EXPECT_EQ(transaction.RecipientAddress, notification.Recipient.unresolved());
				EXPECT_EQ(BalanceTransferNotification::AmountType::Static, notification.TransferAmountType);
			});
		}

		builder.template addExpectation<TransferTokensNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.TokensCount, notification.TokensCount);
			EXPECT_EQ(transaction.TokensPtr(), notification.TokensPtr);
		});

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion

	// region publish - both message and tokens

	namespace {
		template<typename TTransaction>
		void PrepareMessageAndTokensTransaction(TTransaction& transaction) {
			test::FillWithRandomData(transaction);
			transaction.TokensCount = 3;
			transaction.MessageSize = 17;
			transaction.Size = static_cast<uint32_t>(TTransaction::CalculateRealSize(transaction));
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrderWhenMessageAndTokensArePresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(3, 17);
		PrepareMessageAndTokensTransaction(*pTransaction);

		// Act + Assert:
		test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(*pTransaction, {
			InternalPaddingNotification::Notification_Type,
			AccountAddressNotification::Notification_Type,
			AddressInteractionNotification::Notification_Type,
			BalanceTransferNotification::Notification_Type,
			BalanceTransferNotification::Notification_Type,
			BalanceTransferNotification::Notification_Type,
			TransferMessageNotification::Notification_Type,
			TransferTokensNotification::Notification_Type
		});
	}

	PLUGIN_TEST(CanPublishAllNotificationsWhenMessageAndTokensArePresent) {
		// Arrange:
		auto pTransaction = CreateTransactionWithTokens<TTraits>(3, 17);
		PrepareMessageAndTokensTransaction(*pTransaction);

		const auto& transaction = *pTransaction;
		typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
		AddCommonExpectations<TTraits>(builder, transaction);
		for (auto i = 0u; i < 3u; ++i) {
			builder.template addExpectation<BalanceTransferNotification>(i, [&transaction, i](const auto& notification) {
				EXPECT_TRUE(notification.Sender.isResolved());
				EXPECT_FALSE(notification.Recipient.isResolved());

				EXPECT_EQ(GetSignerAddress(transaction), notification.Sender.resolved());
				EXPECT_EQ(transaction.TokensPtr()[i].TokenId, notification.TokenId);
				EXPECT_EQ(transaction.TokensPtr()[i].Amount, notification.Amount);
				EXPECT_EQ(transaction.RecipientAddress, notification.Recipient.unresolved());
				EXPECT_EQ(BalanceTransferNotification::AmountType::Static, notification.TransferAmountType);
			});
		}

		builder.template addExpectation<TransferMessageNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.SignerPublicKey, notification.SenderPublicKey);
			EXPECT_EQ(transaction.RecipientAddress, notification.Recipient);
			EXPECT_EQ(17u, notification.MessageSize);
			EXPECT_EQ(transaction.MessagePtr(), notification.MessagePtr);
		});
		builder.template addExpectation<TransferTokensNotification>([&transaction](const auto& notification) {
			EXPECT_EQ(transaction.TokensCount, notification.TokensCount);
			EXPECT_EQ(transaction.TokensPtr(), notification.TokensPtr);
		});

		// Act + Assert:
		builder.runTest(transaction);
	}

	// endregion
}}
