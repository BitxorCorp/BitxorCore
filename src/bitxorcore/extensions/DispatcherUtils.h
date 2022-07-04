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
#include "bitxorcore/chain/ChainFunctions.h"
#include "bitxorcore/consumers/HashCheckOptions.h"
#include "bitxorcore/disruptor/BatchRangeDispatcher.h"
#include "bitxorcore/thread/Task.h"

namespace bitxorcore {
	namespace config { struct NodeConfiguration; }
	namespace extensions { class ServiceLocator; }
	namespace subscribers { class TransactionStatusSubscriber; }
}

namespace bitxorcore { namespace extensions {

	/// Creates hash check options based on \a cacheDuration and \a nodeConfig.
	consumers::HashCheckOptions CreateHashCheckOptions(const utils::TimeSpan& cacheDuration, const config::NodeConfiguration& nodeConfig);

	/// Converts \a subscriber to a sink.
	chain::FailedTransactionSink SubscriberToSink(subscribers::TransactionStatusSubscriber& subscriber);

	/// Adds dispatcher counters with prefix \a counterPrefix to \a locator for a dispatcher named \a dispatcherName.
	void AddDispatcherCounters(ServiceLocator& locator, const std::string& dispatcherName, const std::string& counterPrefix);

	/// Transaction batch range dispatcher.
	using TransactionBatchRangeDispatcher = disruptor::BatchRangeDispatcher<model::AnnotatedTransactionRange>;

	/// Creates a task with \a name that dispatches all transactions batched in \a dispatcher.
	thread::Task CreateBatchTransactionTask(TransactionBatchRangeDispatcher& dispatcher, const std::string& name);
}}
