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

#include "Transaction.h"
#include "TransactionPlugin.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace model {

	namespace {
		bool IsSizeValidInternal(const Transaction& transaction, const TransactionRegistry& registry) {
			const auto* pPlugin = registry.findPlugin(transaction.Type);
			if (!pPlugin || !pPlugin->supportsTopLevel()) {
				BITXORCORE_LOG(warning)
						<< "rejected transaction with type: " << transaction.Type
						<< (pPlugin ? " (top level not supported)" : "");
				return false;
			}

			return pPlugin->isSizeValid(transaction);
		}
	}

	bool IsSizeValid(const Transaction& transaction, const TransactionRegistry& registry) {
		if (transaction.Size < sizeof(Transaction)) {
			BITXORCORE_LOG(warning) << "transaction failed size validation with size " << transaction.Size;
			return false;
		}

		if (IsSizeValidInternal(transaction, registry))
			return true;

		BITXORCORE_LOG(warning) << transaction.Type << " transaction failed size validation with size " << transaction.Size;
		return false;
	}
}}
