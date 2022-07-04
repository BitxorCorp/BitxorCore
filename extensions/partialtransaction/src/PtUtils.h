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
#include "PtTypes.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/model/WeakCosignedTransactionInfo.h"

namespace bitxorcore { namespace partialtransaction {

	/// Stitches a weak cosigned transaction \a info into a full aggregate transaction.
	std::unique_ptr<model::Transaction> StitchAggregate(const model::WeakCosignedTransactionInfo& transactionInfo);

	/// Splits up cosigned transaction infos (\a transactionInfos) and forwards transactions as a single range
	/// to \a transactionRangeConsumer and cosignatures individually to \a cosignatureConsumer.
	void SplitCosignedTransactionInfos(
			CosignedTransactionInfos&& transactionInfos,
			const consumer<model::TransactionRange&&>& transactionRangeConsumer,
			const consumer<model::DetachedCosignature&&>& cosignatureConsumer);
}}
