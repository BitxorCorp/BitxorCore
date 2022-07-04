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

#include "MetadataTransactionPlugin.h"
#include "src/model/AccountMetadataTransaction.h"
#include "src/model/MetadataNotifications.h"
#include "src/model/TokenMetadataTransaction.h"
#include "src/model/NamespaceMetadataTransaction.h"
#include "plugins/txes/namespace/src/model/NamespaceNotifications.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		UnresolvedPartialMetadataKey ExtractPartialMetadataKey(const TTransaction& transaction, const PublishContext& context) {
			return { context.SignerAddress, transaction.TargetAddress, transaction.ScopedMetadataKey };
		}

		struct AccountTraits {
			template<typename TTransaction>
			static MetadataTarget ExtractMetadataTarget(const TTransaction&) {
				return { MetadataType::Account, 0 };
			}

			template<typename TTransaction>
			static void RaiseCustomNotifications(const TTransaction& transaction, NotificationSubscriber& sub) {
				sub.notify(AccountAddressNotification(transaction.TargetAddress));
			}
		};

		struct TokenTraits {
			template<typename TTransaction>
			static MetadataTarget ExtractMetadataTarget(const TTransaction& transaction) {
				return { MetadataType::Token, transaction.TargetTokenId.unwrap() };
			}

			template<typename TTransaction>
			static void RaiseCustomNotifications(const TTransaction& transaction, NotificationSubscriber& sub) {
				sub.notify(TokenRequiredNotification(transaction.TargetAddress, transaction.TargetTokenId));
			}
		};

		struct NamespaceTraits {
			template<typename TTransaction>
			static MetadataTarget ExtractMetadataTarget(const TTransaction& transaction) {
				return { MetadataType::Namespace, transaction.TargetNamespaceId.unwrap() };
			}

			template<typename TTransaction>
			static void RaiseCustomNotifications(const TTransaction& transaction, NotificationSubscriber& sub) {
				sub.notify(NamespaceRequiredNotification(transaction.TargetAddress, transaction.TargetNamespaceId));
			}
		};

		template<typename TTraits>
		class Publisher {
		public:
			template<typename TTransaction>
			static void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
				sub.notify(MetadataSizesNotification(transaction.ValueSizeDelta, transaction.ValueSize));
				sub.notify(MetadataValueNotification(
						ExtractPartialMetadataKey(transaction, context),
						TTraits::ExtractMetadataTarget(transaction),
						transaction.ValueSizeDelta,
						transaction.ValueSize,
						transaction.ValuePtr()));

				TTraits::RaiseCustomNotifications(transaction, sub);
			}
		};
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(AccountMetadata, Only_Embeddable, Publisher<AccountTraits>::Publish)
	DEFINE_TRANSACTION_PLUGIN_FACTORY(TokenMetadata, Only_Embeddable, Publisher<TokenTraits>::Publish)
	DEFINE_TRANSACTION_PLUGIN_FACTORY(NamespaceMetadata, Only_Embeddable, Publisher<NamespaceTraits>::Publish)
}}
