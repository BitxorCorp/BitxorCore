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

#include "MultisigAccountModificationTransactionPlugin.h"
#include "src/model/MultisigAccountModificationTransaction.h"
#include "src/model/MultisigNotifications.h"
#include "bitxorcore/model/NotificationSubscriber.h"
#include "bitxorcore/model/TransactionPluginFactory.h"

using namespace bitxorcore::model;

namespace bitxorcore { namespace plugins {

	namespace {
		template<typename TTransaction>
		void Publish(const TTransaction& transaction, const PublishContext& context, NotificationSubscriber& sub) {
			// 1. basic
			sub.notify(InternalPaddingNotification(transaction.MultisigAccountModificationTransactionBody_Reserved1));

			// 2. cosig changes
			UnresolvedAddressSet addedCosignatories;
			if (0 < transaction.AddressAdditionsCount || 0 < transaction.AddressDeletionsCount) {
				// - raise new cosignatory notifications first because they are used for multisig loop detection
				// - notify cosignatories' addresses in order to allow added cosignatories to get aggregate notifications
				const auto* pAddressAdditions = transaction.AddressAdditionsPtr();
				for (auto i = 0u; i < transaction.AddressAdditionsCount; ++i) {
					addedCosignatories.insert(pAddressAdditions[i]);

					sub.notify(AccountAddressNotification(pAddressAdditions[i]));
					sub.notify(MultisigNewCosignatoryNotification(context.SignerAddress, pAddressAdditions[i]));
				}

				sub.notify(MultisigCosignatoriesNotification(
						context.SignerAddress,
						transaction.AddressAdditionsCount,
						pAddressAdditions,
						transaction.AddressDeletionsCount,
						transaction.AddressDeletionsPtr()));
			}

			if (!addedCosignatories.empty())
				sub.notify(AddressInteractionNotification(context.SignerAddress, transaction.Type, addedCosignatories));

			// 3. setting changes
			sub.notify(MultisigSettingsNotification(context.SignerAddress, transaction.MinRemovalDelta, transaction.MinApprovalDelta));
		}
	}

	DEFINE_TRANSACTION_PLUGIN_FACTORY(MultisigAccountModification, Only_Embeddable, Publish)
}}
