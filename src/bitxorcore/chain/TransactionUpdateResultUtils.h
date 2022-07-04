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
#include "BatchUpdateResult.h"
#include "bitxorcore/model/EntityInfo.h"

namespace bitxorcore { namespace chain {

	/// Aggregates \a updateResults by count.
	template<typename TUpdateResult>
	BatchUpdateResult AggregateUpdateResults(const std::vector<TUpdateResult>& updateResults) {
		BatchUpdateResult aggregateResult;

		for (const auto& updateResult : updateResults) {
			switch (updateResult.Type) {
			case TUpdateResult::UpdateType::Invalid:
				++aggregateResult.FailureCount;
				break;

			case TUpdateResult::UpdateType::Neutral:
				++aggregateResult.NeutralCount;
				break;

			default:
				++aggregateResult.SuccessCount;
				break;
			}
		}

		return aggregateResult;
	}

	/// Filters \a transactionInfos based on \a updateResults by selecting only valid transactions.
	template<typename TUpdateResult>
	std::vector<model::TransactionInfo> SelectValid(
			std::vector<model::TransactionInfo>&& transactionInfos,
			const std::vector<TUpdateResult>& updateResults) {
		if (transactionInfos.size() != updateResults.size()) {
			std::ostringstream out;
			out
					<< "number of transaction infos " << transactionInfos.size()
					<< " must match number of update results " << updateResults.size();
			BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
		}

		std::vector<model::TransactionInfo> filteredTransactionInfos;

		for (auto i = 0u; i < transactionInfos.size(); ++i) {
			switch (updateResults[i].Type) {
			case TUpdateResult::UpdateType::Invalid:
			case TUpdateResult::UpdateType::Neutral:
				break;

			default:
				filteredTransactionInfos.push_back(std::move(transactionInfos[i]));
				break;
			}
		}

		return filteredTransactionInfos;
	}
}}
