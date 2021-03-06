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

#include "UtUpdater.h"
#include "ChainResults.h"
#include "ProcessContextsBuilder.h"
#include "ProcessingNotificationSubscriber.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache/RelockableDetachedBitxorCoreCache.h"
#include "bitxorcore/cache_tx/UtCache.h"
#include "bitxorcore/model/FeeUtils.h"
#include "bitxorcore/utils/HexFormatter.h"

namespace bitxorcore { namespace chain {

	namespace {
		struct ApplyState {
			constexpr ApplyState(cache::UtCacheModifierProxy& modifier, cache::BitxorCoreCacheDelta& unconfirmedBitxorCoreCache)
					: Modifier(modifier)
					, UnconfirmedBitxorCoreCache(unconfirmedBitxorCoreCache)
			{}

			cache::UtCacheModifierProxy& Modifier;
			cache::BitxorCoreCacheDelta& UnconfirmedBitxorCoreCache;
		};

		class TransactionInfoFormatter {
		public:
			explicit TransactionInfoFormatter(const model::TransactionInfo& transactionInfo) : m_transactionInfo(transactionInfo)
			{}

		public:
			friend std::ostream& operator<<(std::ostream& out, const TransactionInfoFormatter& formatter) {
				const auto& transaction = *formatter.m_transactionInfo.pEntity;
				out
						<< "{ EntityHash " << formatter.m_transactionInfo.EntityHash
						<< ", Type " << transaction.Type
						<< ", Version " << static_cast<uint16_t>(transaction.Version)
						<< ", SignerPublicKey " << transaction.SignerPublicKey
						<< " }";
				return out;
			}

		private:
			const model::TransactionInfo& m_transactionInfo;
		};
	}

	class UtUpdater::Impl final {
	public:
		Impl(
				cache::UtCache& transactionsCache,
				const cache::BitxorCoreCache& confirmedBitxorCoreCache,
				BlockFeeMultiplier minFeeMultiplier,
				const ExecutionConfiguration& executionConfig,
				const TimeSupplier& timeSupplier,
				const FailedTransactionSink& failedTransactionSink,
				const Throttle& throttle)
				: m_transactionsCache(transactionsCache)
				, m_detachedBitxorCoreCache(confirmedBitxorCoreCache)
				, m_minFeeMultiplier(minFeeMultiplier)
				, m_executionConfig(executionConfig)
				, m_timeSupplier(timeSupplier)
				, m_failedTransactionSink(failedTransactionSink)
				, m_throttle(throttle)
		{}

	public:
		std::vector<UtUpdateResult> update(const std::vector<model::TransactionInfo>& utInfos) {
			// 1. lock the UT cache and lock the unconfirmed copy
			auto modifier = m_transactionsCache.modifier();
			auto pUnconfirmedBitxorCoreCache = m_detachedBitxorCoreCache.getAndTryLock();
			if (!pUnconfirmedBitxorCoreCache) {
				// if there is no unconfirmed cache state, it means that a block update is forthcoming
				// just add all to the cache and they will be validated later
				addAll(modifier, utInfos);

				std::vector<UtUpdateResult> updateResults;
				updateResults.reserve(utInfos.size());
				for (auto i = 0u; i < utInfos.size(); ++i)
					updateResults.push_back({ UtUpdateResult::UpdateType::Neutral });

				return updateResults;
			}

			auto applyState = ApplyState(modifier, *pUnconfirmedBitxorCoreCache);
			return apply(applyState, utInfos, TransactionSource::New);
		}

		void update(const utils::HashPointerSet& confirmedTransactionHashes, const std::vector<model::TransactionInfo>& utInfos) {
			if (!confirmedTransactionHashes.empty() || !utInfos.empty()) {
				BITXORCORE_LOG(debug)
						<< "confirmed " << confirmedTransactionHashes.size() << " transactions, "
						<< "reverted " << utInfos.size() << " transactions";
			}

			// 1. lock and clear the UT cache - UT cache must be locked before bitxorcore cache to prevent race condition whereby
			//    other update overload applies transactions to rebased cache before UT lock is held
			auto modifier = m_transactionsCache.modifier();
			auto originalTransactionInfos = modifier.removeAll();

			// 2. lock the bitxorcore cache and rebase the unconfirmed bitxorcore cache
			auto pUnconfirmedBitxorCoreCache = m_detachedBitxorCoreCache.rebaseAndLock();

			// 3. add back reverted txes
			auto applyState = ApplyState(modifier, *pUnconfirmedBitxorCoreCache);
			apply(applyState, utInfos, TransactionSource::Reverted);

			// 4. add back original txes that have not been confirmed
			apply(applyState, originalTransactionInfos, TransactionSource::Existing, [&confirmedTransactionHashes](const auto& info) {
				return confirmedTransactionHashes.cend() == confirmedTransactionHashes.find(&info.EntityHash);
			});
		}

	private:
		std::vector<UtUpdateResult> apply(
				const ApplyState& applyState,
				const std::vector<model::TransactionInfo>& utInfos,
				TransactionSource transactionSource) {
			return apply(applyState, utInfos, transactionSource, [](const auto&) { return true; });
		}

		std::vector<UtUpdateResult> apply(
				const ApplyState& applyState,
				const std::vector<model::TransactionInfo>& utInfos,
				TransactionSource transactionSource,
				const predicate<const model::TransactionInfo&>& filter) {
			std::vector<UtUpdateResult> updateResults;
			updateResults.reserve(utInfos.size());

			// note that the validator and observer context height is one larger than the chain height
			// since the validation and observation has to be for the *next* block
			auto effectiveHeight = m_detachedBitxorCoreCache.height() + Height(1);
			ProcessContextsBuilder contextBuilder(effectiveHeight, m_timeSupplier(), m_executionConfig);
			contextBuilder.setCache(applyState.UnconfirmedBitxorCoreCache);
			auto validatorContext = contextBuilder.buildValidatorContext();
			auto observerContext = contextBuilder.buildObserverContext();

			size_t numRejectedTransactions = 0;
			for (const auto& utInfo : utInfos) {
				const auto& entity = *utInfo.pEntity;
				const auto& entityHash = utInfo.EntityHash;

				if (!filter(utInfo)) {
					updateResults.push_back({ UtUpdateResult::UpdateType::Neutral });
					continue;
				}

				auto minTransactionFee = model::CalculateTransactionFee(m_minFeeMultiplier, entity);
				if (entity.MaxFee < minTransactionFee) {
					// don't log reverted transactions that could have been included by harvester with lower min fee multiplier
					if (TransactionSource::New == transactionSource) {
						BITXORCORE_LOG(debug)
								<< "dropping transaction " << TransactionInfoFormatter(utInfo) << " with max fee " << entity.MaxFee
								<< " because min fee is " << minTransactionFee;
					}

					updateResults.push_back({ UtUpdateResult::UpdateType::Neutral });
					continue;
				}

				if (throttle(utInfo, transactionSource, applyState, validatorContext.Cache)) {
					BITXORCORE_LOG(warning) << "dropping transaction " << TransactionInfoFormatter(utInfo) << " due to throttle";
					m_failedTransactionSink(entity, entityHash, Failure_Chain_Unconfirmed_Cache_Too_Full);
					updateResults.push_back({ UtUpdateResult::UpdateType::Neutral });
					continue;
				}

				if (!applyState.Modifier.add(utInfo)) {
					updateResults.push_back({ UtUpdateResult::UpdateType::Neutral });
					continue;
				}

				// notice that subscriber is created within loop because aggregate result needs to be reset each iteration
				const auto& validator = *m_executionConfig.pValidator;
				const auto& observer = *m_executionConfig.pObserver;
				ProcessingNotificationSubscriber sub(validator, validatorContext, observer, observerContext);
				sub.enableUndo();
				auto entityInfo = model::WeakEntityInfo(entity, entityHash);
				m_executionConfig.pNotificationPublisher->publish(entityInfo, sub);
				if (!IsValidationResultSuccess(sub.result())) {
					BITXORCORE_LOG(trace) << "dropping transaction " << TransactionInfoFormatter(utInfo) << ": " << sub.result();
					++numRejectedTransactions;

					// only forward failure (not neutral) results
					if (IsValidationResultFailure(sub.result()))
						m_failedTransactionSink(entity, entityHash, sub.result());

					sub.undo();
					applyState.Modifier.remove(entityHash);
					updateResults.push_back({ UtUpdateResult::UpdateType::Invalid });
					continue;
				}

				updateResults.push_back({ UtUpdateResult::UpdateType::New });
			}

			if (numRejectedTransactions > 0)
				BITXORCORE_LOG(warning) << "apply dropped " << numRejectedTransactions << " transactions";

			return updateResults;
		}

		bool throttle(
				const model::TransactionInfo& utInfo,
				TransactionSource transactionSource,
				const ApplyState& applyState,
				const cache::ReadOnlyBitxorCoreCache& cache) const {
			return m_throttle(utInfo, { transactionSource, m_detachedBitxorCoreCache.height(), cache, applyState.Modifier });
		}

		void addAll(cache::UtCacheModifierProxy& modifier, const std::vector<model::TransactionInfo>& utInfos) {
			for (const auto& utInfo : utInfos)
				modifier.add(utInfo);
		}

	private:
		cache::UtCache& m_transactionsCache;
		cache::RelockableDetachedBitxorCoreCache m_detachedBitxorCoreCache;
		BlockFeeMultiplier m_minFeeMultiplier;
		ExecutionConfiguration m_executionConfig;
		TimeSupplier m_timeSupplier;
		FailedTransactionSink m_failedTransactionSink;
		UtUpdater::Throttle m_throttle;
	};

	UtUpdater::UtUpdater(
			cache::UtCache& transactionsCache,
			const cache::BitxorCoreCache& confirmedBitxorCoreCache,
			BlockFeeMultiplier minFeeMultiplier,
			const ExecutionConfiguration& executionConfig,
			const TimeSupplier& timeSupplier,
			const FailedTransactionSink& failedTransactionSink,
			const Throttle& throttle)
			: m_pImpl(std::make_unique<Impl>(
					transactionsCache,
					confirmedBitxorCoreCache,
					minFeeMultiplier,
					executionConfig,
					timeSupplier,
					failedTransactionSink,
					throttle))
	{}

	UtUpdater::~UtUpdater() = default;

	std::vector<UtUpdateResult> UtUpdater::update(const std::vector<model::TransactionInfo>& utInfos) {
		return m_pImpl->update(utInfos);
	}

	void UtUpdater::update(const utils::HashPointerSet& confirmedTransactionHashes, const std::vector<model::TransactionInfo>& utInfos) {
		m_pImpl->update(confirmedTransactionHashes, utInfos);
	}
}}
