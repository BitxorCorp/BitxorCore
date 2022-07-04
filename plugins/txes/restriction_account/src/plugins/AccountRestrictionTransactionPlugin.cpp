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

#include "AccountRestrictionTransactionPlugin.h"
#include "src/model/AccountAddressRestrictionTransaction.h"
#include "src/model/AccountTokenRestrictionTransaction.h"
#include "src/model/AccountOperationRestrictionTransaction.h"
#include "src/model/AccountRestrictionNotifications.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		struct AddressTraits {
			using UnresolvedValueType = UnresolvedAddress;
			using ResolvedValueType = Address;
			using ModifyAccountRestrictionsNotification = ModifyAccountAddressRestrictionsNotification;
			using ModifyAccountRestrictionValueNotification = ModifyAccountAddressRestrictionValueNotification;
		};

		struct TokenTraits {
			using UnresolvedValueType = UnresolvedTokenId;
			using ResolvedValueType = TokenId;
			using ModifyAccountRestrictionsNotification = ModifyAccountTokenRestrictionsNotification;
			using ModifyAccountRestrictionValueNotification = ModifyAccountTokenRestrictionValueNotification;
		};

		struct OperationTraits {
			using UnresolvedValueType = EntityType;
			using ResolvedValueType = EntityType;
			using ModifyAccountRestrictionsNotification = ModifyAccountOperationRestrictionsNotification;
			using ModifyAccountRestrictionValueNotification = ModifyAccountOperationRestrictionValueNotification;
		};

		template<typename TValueNotification, typename TTransaction, typename TResolvedValue>
		void RaiseValueNotifications(
				NotificationSubscriber& sub,
				const TTransaction& transaction,
				const PublishContext& context,
				const TResolvedValue* pValues,
				uint8_t numValues,
				AccountRestrictionModificationAction action) {
			for (auto i = 0u; i < numValues; ++i)
				sub.notify(TValueNotification(context.SignerAddress, transaction.RestrictionFlags, pValues[i], action));
		}

		template<typename TTraits>
		class Publisher {
		public:
			template<typename TTransaction>
			static void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
				sub.notify(InternalPaddingNotification(transaction.AccountRestrictionTransactionBody_Reserved1));
				sub.notify(AccountRestrictionModificationNotification(
						transaction.RestrictionFlags,
						transaction.RestrictionAdditionsCount,
						transaction.RestrictionDeletionsCount));
				sub.notify(CreateAccountRestrictionModificationsNotification<TTransaction>(transaction, context));

				using ValueNotification = typename TTraits::ModifyAccountRestrictionValueNotification;
				RaiseValueNotifications<ValueNotification>(
						sub,
						transaction,
						context,
						transaction.RestrictionAdditionsPtr(),
						transaction.RestrictionAdditionsCount,
						AccountRestrictionModificationAction::Add);
				RaiseValueNotifications<ValueNotification>(
						sub,
						transaction,
						context,
						transaction.RestrictionDeletionsPtr(),
						transaction.RestrictionDeletionsCount,
						AccountRestrictionModificationAction::Del);
			}

		private:
			template<typename TTransaction>
			static auto CreateAccountRestrictionModificationsNotification(const TTransaction& transaction, const PublishContext& context) {
				return typename TTraits::ModifyAccountRestrictionsNotification(
						context.SignerAddress,
						transaction.RestrictionFlags,
						transaction.RestrictionAdditionsCount,
						transaction.RestrictionAdditionsPtr(),
						transaction.RestrictionDeletionsCount,
						transaction.RestrictionDeletionsPtr());
			}
		};
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(AccountAddressRestriction, Default, Publisher<AddressTraits>::Publish)
	DEFINE_TRANSACTION_PLUGIN_FACTORY(AccountTokenRestriction, Default, Publisher<TokenTraits>::Publish)
	DEFINE_TRANSACTION_PLUGIN_FACTORY(AccountOperationRestriction, Default, Publisher<OperationTraits>::Publish)
}}
