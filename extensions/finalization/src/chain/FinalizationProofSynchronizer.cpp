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

#include "FinalizationProofSynchronizer.h"
#include "finalization/src/api/RemoteProofApi.h"
#include "finalization/src/io/ProofStorageCache.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/model/HeightGrouping.h"
#include "bitxorcore/model/VotingSet.h"

namespace bitxorcore { namespace chain {

	namespace {
		class ProofSynchronizer {
		private:
			using NodeInteractionFuture = thread::future<ionet::NodeInteractionResultCode>;

		public:
			ProofSynchronizer(
					uint64_t votingSetGrouping,
					BlockDuration unfinalizedBlocksDuration,
					const io::BlockStorageCache& blockStorage,
					io::ProofStorageCache& proofStorage,
					const predicate<const model::FinalizationProof&>& proofValidator)
					: m_votingSetGrouping(votingSetGrouping)
					, m_unfinalizedBlocksDuration(unfinalizedBlocksDuration)
					, m_blockStorage(blockStorage)
					, m_proofStorage(proofStorage)
					, m_proofValidator(proofValidator)
			{}

		public:
			NodeInteractionFuture operator()(const api::RemoteProofApi& api) {
				auto localFinalizedHeight = m_proofStorage.view().statistics().Height;
				auto requestEpoch = calculateRequestEpoch(localFinalizedHeight);
				if (FinalizationEpoch() == requestEpoch) {
					BITXORCORE_LOG(trace)
							<< "skipping proof pull with request epoch " << requestEpoch
							<< " and local finalized height " << localFinalizedHeight;
					return thread::make_ready_future(ionet::NodeInteractionResultCode::Neutral);
				}

				auto startFuture = thread::compose(
					api.finalizationStatistics(),
					[&api, localFinalizedHeight, requestEpoch](auto&& finalizationStatisticsFuture) {
						auto remoteFinalizationStatistics = finalizationStatisticsFuture.get();
						auto shouldPullProof = requestEpoch <= remoteFinalizationStatistics.Round.Epoch
								&& localFinalizedHeight < remoteFinalizationStatistics.Height;
						return shouldPullProof
								? api.proofAt(requestEpoch)
								: thread::make_ready_future(std::shared_ptr<const model::FinalizationProof>());
					});

				auto& proofStorage = m_proofStorage;
				auto proofValidator = m_proofValidator;
				return startFuture.then([&proofStorage, proofValidator, localFinalizedHeight, requestEpoch](auto&& proofFuture) {
					try {
						auto pProof = proofFuture.get();
						if (!pProof) {
							BITXORCORE_LOG(debug)
									<< "could not receive proof with request epoch " << requestEpoch
									<< " and local finalized height " << localFinalizedHeight;
							return ionet::NodeInteractionResultCode::Neutral;
						}

						BITXORCORE_LOG(debug) << "peer returned proof for round " << pProof->Round << " at height " << pProof->Height;

						if (requestEpoch != pProof->Round.Epoch || localFinalizedHeight >= pProof->Height) {
							BITXORCORE_LOG(warning)
									<< "peer returned unexpected proof for round " << pProof->Round << " at height " << pProof->Height
									<< " (requested epoch " << requestEpoch << " at height " << localFinalizedHeight << ")";
							return ionet::NodeInteractionResultCode::Failure;
						}

						if (!proofValidator(*pProof)) {
							BITXORCORE_LOG(warning) << "peer returned proof that failed validation";
							return ionet::NodeInteractionResultCode::Failure;
						}

						BITXORCORE_LOG(debug)
								<< "saving proof for round " << pProof->Round << " at height " << pProof->Height
								<< " with hash " << pProof->Hash;
						proofStorage.modifier().saveProof(*pProof);
						return ionet::NodeInteractionResultCode::Success;
					} catch (const bitxorcore_runtime_error& e) {
						BITXORCORE_LOG(warning)
								<< "exception thrown while requesting proof for epoch " << requestEpoch
								<< ": " << e.what();
						return ionet::NodeInteractionResultCode::Failure;
					}
				});
			}

		private:
			FinalizationEpoch calculateRequestEpoch(Height localFinalizedHeight) {
				auto localChainHeight = m_blockStorage.view().chainHeight();
				auto nextProofHeight = model::CalculateGroupedHeight<Height>(
						Height(localFinalizedHeight.unwrap() + m_votingSetGrouping + 1),
						m_votingSetGrouping);

				if (nextProofHeight < localChainHeight)
					return model::CalculateFinalizationEpochForHeight(nextProofHeight, m_votingSetGrouping);

				auto unfinalizedBlocksDuration = BlockDuration((localChainHeight - localFinalizedHeight).unwrap());
				if (BlockDuration() != m_unfinalizedBlocksDuration && unfinalizedBlocksDuration >= m_unfinalizedBlocksDuration) {
					auto epoch = model::CalculateFinalizationEpochForHeight(localFinalizedHeight, m_votingSetGrouping);
					return model::CalculateVotingSetEndHeight(epoch, m_votingSetGrouping) != localFinalizedHeight
							? epoch
							: epoch + FinalizationEpoch(1);
				}

				return FinalizationEpoch();
			}

		private:
			uint64_t m_votingSetGrouping;
			BlockDuration m_unfinalizedBlocksDuration;
			const io::BlockStorageCache& m_blockStorage;
			io::ProofStorageCache& m_proofStorage;
			predicate<const model::FinalizationProof&> m_proofValidator;
		};
	}

	RemoteNodeSynchronizer<api::RemoteProofApi> CreateFinalizationProofSynchronizer(
			uint64_t votingSetGrouping,
			BlockDuration unfinalizedBlocksDuration,
			const io::BlockStorageCache& blockStorage,
			io::ProofStorageCache& proofStorage,
			const predicate<const model::FinalizationProof&>& proofValidator) {
		return ProofSynchronizer(votingSetGrouping, unfinalizedBlocksDuration, blockStorage, proofStorage, proofValidator);
	}
}}
