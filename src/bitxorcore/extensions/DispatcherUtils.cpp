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

#include "DispatcherUtils.h"
#include "ServiceLocator.h"
#include "bitxorcore/config/NodeConfiguration.h"
#include "bitxorcore/disruptor/ConsumerDispatcher.h"
#include "bitxorcore/subscribers/TransactionStatusSubscriber.h"
#include "bitxorcore/utils/TimeSpan.h"

namespace bitxorcore { namespace extensions {

	consumers::HashCheckOptions CreateHashCheckOptions(const utils::TimeSpan& cacheDuration, const config::NodeConfiguration& nodeConfig) {
		return consumers::HashCheckOptions(
				cacheDuration.millis(),
				nodeConfig.ShortLivedCachePruneInterval.millis(),
				nodeConfig.ShortLivedCacheMaxSize);
	}

	chain::FailedTransactionSink SubscriberToSink(subscribers::TransactionStatusSubscriber& subscriber) {
		return [&subscriber](const auto& transaction, const auto& hash, auto result) {
			subscriber.notifyStatus(transaction, hash, utils::to_underlying_type(result));
		};
	}

	void AddDispatcherCounters(ServiceLocator& locator, const std::string& dispatcherName, const std::string& counterPrefix) {
		using disruptor::ConsumerDispatcher;

		locator.registerServiceCounter<ConsumerDispatcher>(dispatcherName, counterPrefix + " ELEM TOT", [](const auto& dispatcher) {
			return dispatcher.numAddedElements();
		});
		locator.registerServiceCounter<ConsumerDispatcher>(dispatcherName, counterPrefix + " ELEM ACT", [](const auto& dispatcher) {
			return dispatcher.numActiveElements();
		});
		locator.registerServiceCounter<ConsumerDispatcher>(dispatcherName, counterPrefix + " ELEM MEM", [](const auto& dispatcher) {
			return dispatcher.memorySize().megabytes();
		});
	}

	thread::Task CreateBatchTransactionTask(TransactionBatchRangeDispatcher& dispatcher, const std::string& name) {
		return thread::CreateNamedTask("batch " + name + " task", [&dispatcher]() {
			dispatcher.dispatch();
			return thread::make_ready_future(thread::TaskResult::Continue);
		});
	}
}}
