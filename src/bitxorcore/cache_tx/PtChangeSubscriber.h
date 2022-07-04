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
#include "bitxorcore/model/ContainerTypes.h"
#include "bitxorcore/model/EntityInfo.h"

namespace bitxorcore { namespace model { struct Cosignature; } }

namespace bitxorcore { namespace cache {

	/// Partial transactions change subscriber.
	class PLUGIN_API_DEPENDENCY PtChangeSubscriber {
	public:
		using TransactionInfos = model::TransactionInfosSet;

	public:
		virtual ~PtChangeSubscriber() = default;

	public:
		/// Indicates transaction infos (\a transactionInfos) were added to partial transactions.
		/// \note This is only aggregate part and will not have any cosignatures.
		virtual void notifyAddPartials(const TransactionInfos& transactionInfos) = 0;

		/// Indicates \a cosignature was added to a partial transaction (\a parentTransactionInfo).
		virtual void notifyAddCosignature(const model::TransactionInfo& parentTransactionInfo, const model::Cosignature& cosignature) = 0;

		/// Indicates transaction infos (\a transactionInfos) were removed from partial transactions.
		/// \note This is only aggregate part and will not have any cosignatures.
		virtual void notifyRemovePartials(const TransactionInfos& transactionInfos) = 0;

		/// Flushes all pending partial transactions changes.
		virtual void flush() = 0;
	};
}}
