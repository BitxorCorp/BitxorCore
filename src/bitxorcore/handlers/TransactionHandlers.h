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

#pragma once
#include "HandlerTypes.h"
#include "bitxorcore/ionet/PacketHandlers.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/model/Transaction.h"
#include "bitxorcore/utils/ShortHash.h"
#include <unordered_set>

namespace bitxorcore { namespace handlers {

	/// Transactions returned by the unconfirmed transactions retriever.
	using UnconfirmedTransactions = std::vector<std::shared_ptr<const model::Transaction>>;

	/// Prototype for a function that retrieves unconfirmed transactions given a filter and a set of short hashes.
	using UtRetriever = std::function<UnconfirmedTransactions (Timestamp, BlockFeeMultiplier, const utils::ShortHashesSet&)>;

	/// Registers a push transactions handler in \a handlers that forwards transactions to \a transactionRangeHandler
	/// given a transaction \a registry composed of known transactions.
	void RegisterPushTransactionsHandler(
			ionet::ServerPacketHandlers& handlers,
			const model::TransactionRegistry& registry,
			const TransactionRangeHandler& transactionRangeHandler);

	/// Registers a pull transactions handler in \a handlers that responds with unconfirmed transactions
	/// returned by the retriever (\a utRetriever).
	void RegisterPullTransactionsHandler(ionet::ServerPacketHandlers& handlers, const UtRetriever& utRetriever);
}}
