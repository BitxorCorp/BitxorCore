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

#include "PredicateUtils.h"
#include "TransactionSpamThrottle.h"
#include "bitxorcore/cache_tx/UtCache.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"

namespace bitxorcore { namespace sync {

	model::MatchingEntityPredicate ToRequiresValidationPredicate(const chain::KnownHashPredicate& knownHashPredicate) {
		return [knownHashPredicate](auto entityType, auto timestamp, const auto& hash) {
			auto isTransaction = model::BasicEntityType::Transaction == entityType;
			return !isTransaction || !knownHashPredicate(timestamp, hash);
		};
	}

	namespace {
		bool IsBondedTransaction(const model::Transaction& transaction) {
			return transaction.Type == model::MakeEntityType(model::BasicEntityType::Transaction, model::FacilityCode::Aggregate, 2);
		}

		chain::UtUpdater::Throttle CreateDefaultUtUpdaterThrottle(utils::FileSize maxCacheSize) {
			return [maxCacheSize](const auto&, const auto& context) {
				return context.TransactionsCache.memorySize() >= maxCacheSize;
			};
		}
	}

	chain::UtUpdater::Throttle CreateUtUpdaterThrottle(const config::BitxorCoreConfiguration& config) {
		SpamThrottleConfiguration throttleConfig(
				config.Node.TransactionSpamThrottlingMaxBoostFee,
				config.Blockchain.TotalChainImportance,
				config.Node.UnconfirmedTransactionsCacheMaxSize,
				config.Blockchain.MaxTransactionsPerBlock);

		return config.Node.EnableTransactionSpamThrottling
				? CreateTransactionSpamThrottle(throttleConfig, IsBondedTransaction)
				: CreateDefaultUtUpdaterThrottle(throttleConfig.MaxCacheSize);
	}
}}
