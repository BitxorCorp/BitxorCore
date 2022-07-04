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

#include "src/plugins/TokenSupplyRevocationTransactionPlugin.h"
#include "src/model/TokenFlags.h"
#include "src/model/TokenSupplyRevocationTransaction.h"
#include "bitxorcore/utils/HexParser.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"
#include "tests/TestHarness.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

#define TEST_CLASS TokenSupplyRevocationTransactionPluginTests

	// region test utils

	namespace {
		constexpr auto Genesis_Address = "0000CFBB0D4624790ABA656C3DB22278C39CB6C30E41F352";
		constexpr auto Genesis_Public_Key = "2E33898782FCC2A13A3208257C5B3DAC35B36304E323E7E4CB371A57C279418E";

		DEFINE_TRANSACTION_PLUGIN_WITH_CONFIG_TEST_TRAITS(TokenSupplyRevocation, Address, 1, 1,)
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(
			TEST_CLASS,
			,
			,
			Entity_Type_Token_Supply_Revocation,
			utils::ParseByteArray<Address>(Genesis_Address))

	// endregion

	// region publish

	namespace {
		template<typename TTraits>
		void AssertCanPublishAllNotificationsInCorrectOrder(const Key& signerPublicKey) {
			// Arrange:
			typename TTraits::TransactionType transaction;
			test::FillWithRandomData(transaction);
			transaction.Network = NetworkIdentifier::Zero;
			transaction.SignerPublicKey = signerPublicKey;

			// Act + Assert:
			test::TransactionPluginTestUtils<TTraits>::AssertNotificationTypes(transaction, {
				TokenRequiredNotification::Notification_Type,
				BalanceTransferNotification::Notification_Type
			}, utils::ParseByteArray<Address>(Genesis_Address));
		}
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrder_GenesisSigner) {
		AssertCanPublishAllNotificationsInCorrectOrder<TTraits>(utils::ParseByteArray<Key>(Genesis_Public_Key));
	}

	PLUGIN_TEST(CanPublishAllNotificationsInCorrectOrder_NotGenesisSigner) {
		AssertCanPublishAllNotificationsInCorrectOrder<TTraits>(test::GenerateRandomByteArray<Key>());
	}

	namespace {
		template<typename TTraits>
		void AssertCanPublishAllNotifications(const Key& signerPublicKey, TokenFlags expectedPropertyFlagMask) {
			// Arrange:
			typename TTraits::TransactionType transaction;
			test::FillWithRandomData(transaction);
			transaction.Network = NetworkIdentifier::Zero;
			transaction.SignerPublicKey = signerPublicKey;

			typename test::TransactionPluginTestUtils<TTraits>::PublishTestBuilder builder;
			builder.template addExpectation<TokenRequiredNotification>([&transaction, expectedPropertyFlagMask](
					const auto& notification) {
				EXPECT_TRUE(notification.Owner.isResolved());
				EXPECT_FALSE(notification.TokenId.isResolved());

				EXPECT_EQ(GetSignerAddress(transaction), notification.Owner.resolved());
				EXPECT_EQ(transaction.Token.TokenId, notification.TokenId.unresolved());
				EXPECT_EQ(utils::to_underlying_type(expectedPropertyFlagMask), notification.PropertyFlagMask);
			});
			builder.template addExpectation<BalanceTransferNotification>([&transaction](const auto& notification) {
				EXPECT_FALSE(notification.Sender.isResolved());
				EXPECT_TRUE(notification.Recipient.isResolved());

				EXPECT_EQ(transaction.SourceAddress, notification.Sender.unresolved());
				EXPECT_EQ(GetSignerAddress(transaction), notification.Recipient.resolved());
				EXPECT_EQ(transaction.Token.TokenId, notification.TokenId);
				EXPECT_EQ(transaction.Token.Amount, notification.Amount);
				EXPECT_EQ(BalanceTransferNotification::AmountType::Static, notification.TransferAmountType);
			});

			// Act + Assert:
			builder.runTest(transaction, utils::ParseByteArray<Address>(Genesis_Address));
		}
	}

	PLUGIN_TEST(CanPublishAllNotifications_GenesisSigner) {
		AssertCanPublishAllNotifications<TTraits>(utils::ParseByteArray<Key>(Genesis_Public_Key), TokenFlags::None);
	}

	PLUGIN_TEST(CanPublishAllNotifications_NotGenesisSigner) {
		AssertCanPublishAllNotifications<TTraits>(test::GenerateRandomByteArray<Key>(), TokenFlags::Revokable);
	}

	// endregion
}}
