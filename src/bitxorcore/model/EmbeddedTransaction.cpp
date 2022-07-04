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

#include "EmbeddedTransaction.h"
#include "Address.h"
#include "NotificationSubscriber.h"
#include "Transaction.h"
#include "TransactionPlugin.h"
#include "bitxorcore/utils/IntegerMath.h"

namespace bitxorcore { namespace model {

	std::ostream& operator<<(std::ostream& out, const EmbeddedTransaction& transaction) {
		auto version = static_cast<uint16_t>(transaction.Version);
		out << "(embedded) " << transaction.Type << " (v" << version << ") with size " << transaction.Size;
		return out;
	}

	Address GetSignerAddress(const EmbeddedTransaction& transaction) {
		return PublicKeyToAddress(transaction.SignerPublicKey, transaction.Network);
	}

	namespace {
		bool IsSizeValidInternal(const EmbeddedTransaction& transaction, const TransactionRegistry& registry) {
			const auto* pPlugin = registry.findPlugin(transaction.Type);
			if (!pPlugin || !pPlugin->supportsEmbedding()) {
				BITXORCORE_LOG(warning) << "rejected embedded transaction with type: " << transaction.Type;
				return false;
			}

			return pPlugin->embeddedPlugin().isSizeValid(transaction);
		}
	}

	bool IsSizeValid(const EmbeddedTransaction& transaction, const TransactionRegistry& registry) {
		if (transaction.Size < sizeof(EmbeddedTransaction)) {
			BITXORCORE_LOG(warning) << "transaction failed size validation with size " << transaction.Size;
			return false;
		}

		if (IsSizeValidInternal(transaction, registry))
			return true;

		BITXORCORE_LOG(warning) << transaction.Type << " transaction failed size validation with size " << transaction.Size;
		return false;
	}

	void PublishNotifications(const EmbeddedTransaction& transaction, NotificationSubscriber& sub) {
		sub.notify(AccountPublicKeyNotification(transaction.SignerPublicKey));
	}

	const model::EmbeddedTransaction* AdvanceNext(const model::EmbeddedTransaction* pTransaction) {
		const auto* pTransactionData = reinterpret_cast<const uint8_t*>(pTransaction);
		auto paddingSize = utils::GetPaddingSize(pTransaction->Size, 8);
		return reinterpret_cast<const model::EmbeddedTransaction*>(pTransactionData + pTransaction->Size + paddingSize);
	}
}}
