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

#include "FinalizationService.h"
#include "FinalizationBootstrapperService.h"
#include "FinalizationContextFactory.h"
#include "finalization/src/api/RemoteFinalizationApi.h"
#include "finalization/src/api/RemoteProofApi.h"
#include "finalization/src/chain/FinalizationMessageSynchronizer.h"
#include "finalization/src/chain/FinalizationProofSynchronizer.h"
#include "finalization/src/chain/FinalizationProofVerifier.h"
#include "finalization/src/chain/MultiRoundMessageAggregator.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/extensions/NetworkUtils.h"
#include "bitxorcore/extensions/PeersConnectionTasks.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/extensions/SynchronizerTaskCallbacks.h"
#include "bitxorcore/net/PacketWriters.h"
#include "bitxorcore/thread/MultiServicePool.h"

namespace bitxorcore { namespace finalization {

	namespace {
		constexpr auto Service_Name = "fin.writers";
		constexpr auto Service_Id = ionet::ServiceIdentifier(0x46494E4C);

		// region shims for CreateChainSyncAwareSynchronizerTaskCallback

		std::unique_ptr<api::RemoteFinalizationApi> CreateRemoteFinalizationApi(
				ionet::PacketIo& io,
				const model::NodeIdentity& remoteIdentity,
				const model::TransactionRegistry&) {
			return api::CreateRemoteFinalizationApi(io, remoteIdentity);
		}

		std::unique_ptr<api::RemoteProofApi> CreateRemoteProofApi(
				ionet::PacketIo& io,
				const model::NodeIdentity& remoteIdentity,
				const model::TransactionRegistry&) {
			return api::CreateRemoteProofApi(io, remoteIdentity);
		}

		// endregion

		// region tasks

		thread::Task CreateConnectPeersTask(extensions::ServiceState& state, net::PacketWriters& packetWriters) {
			auto settings = extensions::CreateOutgoingSelectorSettings(state, Service_Id, ionet::NodeRoles::Voting);
			auto task = extensions::CreateConnectPeersTask(settings, packetWriters);
			task.Name += " for service Finalization";
			return task;
		}

		thread::Task CreatePullMessagesTask(
				extensions::ServiceLocator& locator,
				const extensions::ServiceState& state,
				net::PacketWriters& packetWriters) {
			const auto& messageAggregator = GetMultiRoundMessageAggregator(locator);
			const auto& serverHooks = GetFinalizationServerHooks(locator);
			auto finalizationMessageSynchronizer = chain::CreateFinalizationMessageSynchronizer(
					[&messageAggregator]() {
						auto messageAggregatorView = messageAggregator.view();
						auto roundRange = model::FinalizationRoundRange(
								messageAggregatorView.minFinalizationRound(),
								messageAggregatorView.maxFinalizationRound());
						BITXORCORE_LOG(trace) << "requesting finalization messages within range " << roundRange;
						return std::make_pair(roundRange, messageAggregatorView.shortHashes());
					},
					serverHooks.messageRangeConsumer());

			thread::Task task;
			task.Name = "pull finalization messages task";
			task.Callback = CreateChainSyncAwareSynchronizerTaskCallback(
					std::move(finalizationMessageSynchronizer),
					CreateRemoteFinalizationApi,
					packetWriters,
					state,
					task.Name);
			return task;
		}

		thread::Task CreatePullProofTask(
				const FinalizationConfiguration& config,
				extensions::ServiceLocator& locator,
				const extensions::ServiceState& state,
				net::PacketWriters& packetWriters) {
			FinalizationContextFactory finalizationContextFactory(config, state);
			auto finalizationProofSynchronizer = chain::CreateFinalizationProofSynchronizer(
					state.config().Blockchain.VotingSetGrouping,
					config.UnfinalizedBlocksDuration.blocks(state.config().Blockchain.BlockGenerationTargetTime),
					state.storage(),
					GetProofStorageCache(locator),
					[finalizationContextFactory](const auto& proof) {
						auto result = chain::VerifyFinalizationProof(proof, finalizationContextFactory.create(proof.Round.Epoch));
						if (chain::VerifyFinalizationProofResult::Success != result) {
							BITXORCORE_LOG(warning)
									<< "proof for round " << proof.Round << " at height " << proof.Height
									<< " failed verification with " << result;
							return false;
						}

						return true;
					});

			thread::Task task;
			task.Name = "pull finalization proof task";
			task.Callback = CreateChainSyncAwareSynchronizerTaskCallback(
					std::move(finalizationProofSynchronizer),
					CreateRemoteProofApi,
					packetWriters,
					state,
					task.Name);
			return task;
		}

		// endregion

		class FinalizationServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			explicit FinalizationServiceRegistrar(const FinalizationConfiguration& config) : m_config(config)
			{}

		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "Finalization", extensions::ServiceRegistrarPhase::Post_Extended_Range_Consumers };
			}

			void registerServiceCounters(extensions::ServiceLocator& locator) override {
				locator.registerServiceCounter<net::PacketWriters>(Service_Name, "FIN WRITERS", [](const auto& writers) {
					return writers.numActiveWriters();
				});
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				auto connectionSettings = extensions::GetConnectionSettings(state.config());
				auto pServiceGroup = state.pool().pushServiceGroup("finalization");
				auto pWriters = pServiceGroup->pushService(net::CreatePacketWriters, locator.keys().caPublicKey(), connectionSettings);

				locator.registerService(Service_Name, pWriters);
				state.packetIoPickers().insert(*pWriters, ionet::NodeRoles::Voting);

				// add sinks
				state.hooks().addBannedNodeIdentitySink(extensions::CreateCloseConnectionSink(*pWriters));

				// add tasks
				state.tasks().push_back(CreateConnectPeersTask(state, *pWriters));
				state.tasks().push_back(CreatePullProofTask(m_config, locator, state, *pWriters));

				if (m_config.EnableVoting)
					state.tasks().push_back(CreatePullMessagesTask(locator, state, *pWriters));
			}

		private:
			FinalizationConfiguration m_config;
		};
	}

	DECLARE_SERVICE_REGISTRAR(Finalization)(const FinalizationConfiguration& config) {
		return std::make_unique<FinalizationServiceRegistrar>(config);
	}
}}
