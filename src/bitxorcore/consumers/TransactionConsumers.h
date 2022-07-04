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
#include "HashCheckOptions.h"
#include "InputUtils.h"
#include "bitxorcore/chain/BatchUpdateResult.h"
#include "bitxorcore/chain/ChainFunctions.h"
#include "bitxorcore/crypto/Signer.h"
#include "bitxorcore/disruptor/DisruptorConsumer.h"
#include "bitxorcore/model/EntityInfo.h"
#include "bitxorcore/validators/ParallelValidationPolicy.h"

namespace bitxorcore { namespace model { class NotificationPublisher; } }

namespace bitxorcore { namespace consumers {

	/// Creates a consumer that calculates hashes of all entities using \a transactionRegistry for the network with the specified
	/// generation hash seed (\a generationHashSeed).
	disruptor::TransactionConsumer CreateTransactionHashCalculatorConsumer(
			const GenerationHashSeed& generationHashSeed,
			const model::TransactionRegistry& transactionRegistry);

	/// Creates a consumer that checks entities for previous processing based on their hash.
	/// \a timeSupplier is used for generating timestamps and \a options specifies additional cache options.
	/// \a knownHashPredicate returns \c true for known hashes.
	disruptor::TransactionConsumer CreateTransactionHashCheckConsumer(
			const chain::TimeSupplier& timeSupplier,
			const HashCheckOptions& options,
			const chain::KnownHashPredicate& knownHashPredicate);

	/// Creates a consumer that runs stateless validation using \a pValidationPolicy and calls \a failedTransactionSink for each failure.
	disruptor::TransactionConsumer CreateTransactionStatelessValidationConsumer(
			const std::shared_ptr<const validators::ParallelValidationPolicy>& pValidationPolicy,
			const chain::FailedTransactionSink& failedTransactionSink);

	/// Creates a consumer that runs batch signature validation using \a pPublisher and \a pool for the network with the specified
	/// generation hash seed (\a generationHashSeed) and calls \a failedTransactionSink for each failure.
	/// \a randomFiller is used to generate random bytes.
	disruptor::TransactionConsumer CreateTransactionBatchSignatureConsumer(
			const GenerationHashSeed& generationHashSeed,
			const crypto::RandomFiller& randomFiller,
			const std::shared_ptr<const model::NotificationPublisher>& pPublisher,
			thread::IoThreadPool& pool,
			const chain::FailedTransactionSink& failedTransactionSink);

	/// Prototype for a function that is called with new transactions.
	using NewTransactionsProcessor = std::function<chain::BatchUpdateResult (TransactionInfos&&)>;

	/// Creates a consumer that calls \a newTransactionsProcessor with all new transactions that will conditionally ban based on
	/// \a minTransactionFailuresCountForBan and \a minTransactionFailuresPercentForBan.
	/// \note This consumer must be last because it destroys the input.
	disruptor::DisruptorConsumer CreateNewTransactionsConsumer(
			uint32_t minTransactionFailuresCountForBan,
			uint32_t minTransactionFailuresPercentForBan,
			const NewTransactionsProcessor& newTransactionsProcessor);
}}
