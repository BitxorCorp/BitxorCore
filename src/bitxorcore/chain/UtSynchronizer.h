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
#include "ChainFunctions.h"
#include "RemoteNodeSynchronizer.h"
#include "bitxorcore/handlers/HandlerTypes.h"
#include "bitxorcore/model/RangeTypes.h"

namespace bitxorcore { namespace api { class RemoteTransactionApi; } }

namespace bitxorcore { namespace chain {

	/// Function signature for supplying a range of short hashes.
	using ShortHashesSupplier = supplier<model::ShortHashRange>;

	/// Creates an unconfirmed transactions synchronizer around the specified time supplier (\a timeSupplier),
	/// short hashes supplier (\a shortHashesSupplier) and transaction range consumer (\a transactionRangeConsumer)
	/// for transactions with fee multipliers at least \a minFeeMultiplier.
	/// \note Remote operation is only initiated when \a shouldExecute returns \c true.
	RemoteNodeSynchronizer<api::RemoteTransactionApi> CreateUtSynchronizer(
			BlockFeeMultiplier minFeeMultiplier,
			const TimeSupplier& timeSupplier,
			const ShortHashesSupplier& shortHashesSupplier,
			const handlers::TransactionRangeHandler& transactionRangeConsumer,
			const predicate<>& shouldExecute);
}}
