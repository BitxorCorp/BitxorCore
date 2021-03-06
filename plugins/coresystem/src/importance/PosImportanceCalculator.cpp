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

#include "ImportanceCalculator.h"
#include "CalculatorUtils.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/model/HeightGrouping.h"
#include "bitxorcore/state/AccountImportanceSnapshots.h"
#include "bitxorcore/utils/StackLogger.h"
#include <boost/multiprecision/cpp_int.hpp>
#include <memory>
#include <vector>

namespace bitxorcore { namespace importance {

	namespace {
		class PosImportanceCalculator final : public ImportanceCalculator {
		public:
			explicit PosImportanceCalculator(const model::BlockchainConfiguration& config) : m_config(config)
			{}

		public:
			void recalculate(
					ImportanceRollbackMode,
					model::ImportanceHeight importanceHeight,
					cache::AccountStateCacheDelta& cache) const override {
				utils::StackLogger stopwatch("PosImportanceCalculator::recalculate", utils::LogLevel::debug);

				// 1. get high value accounts (notice two step lookup because only const iteration is supported)
				const auto& highValueAccounts = cache.highValueAccounts();
				const auto& highValueAddresses = highValueAccounts.addresses();
				std::vector<AccountSummary> accountSummaries;
				accountSummaries.reserve(highValueAddresses.size());

				// 2. calculate sums
				auto importanceGrouping = m_config.ImportanceGrouping;
				ImportanceCalculationContext context;
				auto tokenId = m_config.HarvestingTokenId;
				for (const auto& address : highValueAddresses) {
					auto accountStateIter = cache.find(address);
					auto& accountState = accountStateIter.get();
					const auto& activityBuckets = accountState.ActivityBuckets;
					auto accountActivitySummary = SummarizeAccountActivity(importanceHeight, importanceGrouping, activityBuckets);
					accountSummaries.push_back(AccountSummary(accountActivitySummary, accountState));
					context.ActiveHarvestingTokens = context.ActiveHarvestingTokens + accountState.Balances.get(tokenId);
					context.TotalBeneficiaryCount += accountActivitySummary.BeneficiaryCount;
					context.TotalFeesPaid = context.TotalFeesPaid + accountActivitySummary.TotalFeesPaid;
				}

				// 3. calculate importance parts
				Importance totalActivityImportance;
				for (auto& accountSummary : accountSummaries) {
					CalculateImportances(accountSummary, context, m_config);
					totalActivityImportance = totalActivityImportance + accountSummary.ActivityImportance;
				}

				// 4. calculate the final importance
				auto targetActivityImportanceRaw = m_config.TotalChainImportance.unwrap() * m_config.ImportanceActivityPercentage / 100;
				for (auto& accountSummary : accountSummaries) {
					auto importance = calculateFinalImportance(accountSummary, totalActivityImportance, targetActivityImportanceRaw);
					auto& accountState = *accountSummary.pAccountState;
					FinalizeAccountActivity(importanceHeight, importance, accountState.ActivityBuckets);
					auto effectiveImportance = model::ImportanceHeight(1) == importanceHeight
							? importance
							: Importance(std::min(importance.unwrap(), accountSummary.ActivitySummary.PreviousImportance.unwrap()));
					accountSummary.pAccountState->ImportanceSnapshots.set(effectiveImportance, importanceHeight);
				}

				BITXORCORE_LOG(debug)
						<< "recalculated importances (" << highValueAddresses.size() << " / " << cache.size() << " eligible)"
						<< " at height " << importanceHeight;

				// 5. disable collection of activity for the removed accounts
				cache.processHighValueRemovedAccounts(importanceHeight);
			}

		private:
			Importance calculateFinalImportance(
					const AccountSummary& accountSummary,
					Importance totalActivityImportance,
					Importance::ValueType targetActivityImportanceRaw) const {
				if (Importance() == totalActivityImportance) {
					return 0 < m_config.ImportanceActivityPercentage
							? Importance(accountSummary.StakeImportance.unwrap() * 100 / (100 - m_config.ImportanceActivityPercentage))
							: accountSummary.StakeImportance;
				}

				auto numerator = accountSummary.ActivityImportance.unwrap() * targetActivityImportanceRaw;
				return accountSummary.StakeImportance + Importance(numerator / totalActivityImportance.unwrap());
			}

		private:
			const model::BlockchainConfiguration m_config;
		};
	}

	std::unique_ptr<ImportanceCalculator> CreateImportanceCalculator(const model::BlockchainConfiguration& config) {
		return std::make_unique<PosImportanceCalculator>(config);
	}
}}
