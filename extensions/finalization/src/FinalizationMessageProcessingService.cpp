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

#include "FinalizationMessageProcessingService.h"
#include "FinalizationBootstrapperService.h"
#include "finalization/src/FinalizationConfiguration.h"
#include "finalization/src/chain/MultiRoundMessageAggregator.h"
#include "finalization/src/ionet/FinalizationMessagePacketUtils.h"
#include "finalization/src/model/FinalizationRoundRange.h"
#include "bitxorcore/consumers/RecentHashCache.h"
#include "bitxorcore/extensions/DispatcherUtils.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/extensions/ServiceUtils.h"
#include "bitxorcore/thread/MultiServicePool.h"
#include "bitxorcore/utils/ThrottleLogger.h"

namespace bitxorcore { namespace finalization {

	namespace {
		using MessagesSink = consumer<const ionet::FinalizationMessages&>;

		constexpr auto Writers_Service_Name = "fin.writers";

		auto CreateNewMessagesSink(const extensions::ServiceLocator& locator) {
			return extensions::CreatePushEntitySink<MessagesSink>(locator, Writers_Service_Name);
		}

		auto CreateFinalizationRoundRange(const chain::MultiRoundMessageAggregator& messageAggregator) {
			auto view = messageAggregator.view();
			return model::FinalizationRoundRange(view.minFinalizationRound(), view.maxFinalizationRound());
		}

		class FinalizationMessageProcessingServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			explicit FinalizationMessageProcessingServiceRegistrar(const FinalizationConfiguration& config) : m_config(config)
			{}

		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "FinalizationMessageProcessing", extensions::ServiceRegistrarPhase::Post_Range_Consumers_Phase_Two };
			}

			void registerServiceCounters(extensions::ServiceLocator&) override {
				// no counters
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				auto& messageAggregator = GetMultiRoundMessageAggregator(locator);
				auto& messageProcessingPool = *state.pool().pushIsolatedPool("messageProcessing");

				// register hooks
				auto pRecentHashCache = std::make_shared<consumers::SynchronizedRecentHashCache>(
						state.timeSupplier(),
						extensions::CreateHashCheckOptions(m_config.ShortLivedCacheMessageDuration, state.config().Node));

				auto messagesSink = CreateNewMessagesSink(locator);
				auto& hooks = GetFinalizationServerHooks(locator);
				hooks.setMessageRangeConsumer([&messageAggregator, &messageProcessingPool, pRecentHashCache, messagesSink](
						auto&& messages) {
					auto newMessages = ionet::FinalizationMessages();
					auto extractedMessages = model::FinalizationMessageRange::ExtractEntitiesFromRange(std::move(messages.Range));
					BITXORCORE_LOG(trace) << "received " << extractedMessages.size() << " messages from peer " << messages.SourceIdentity;

					auto roundRange = CreateFinalizationRoundRange(messageAggregator);
					for (const auto& pMessage : extractedMessages) {
						// ignore messages associated with an out of range finalization round
						auto messageRound = pMessage->StepIdentifier.Round();
						if (!model::IsInRange(roundRange, messageRound)) {
							BITXORCORE_LOG_THROTTLE(debug, utils::TimeSpan::FromSeconds(30).millis())
									<< "skipping message with out of bounds round " << messageRound
									<< " when current range is " << roundRange;
							continue;
						}

						auto messageHash = model::CalculateMessageHash(*pMessage);
						if (!pRecentHashCache->add(messageHash))
							continue;

						messageProcessingPool.ioContext().dispatch([&messageAggregator, pMessage, messageHash]() {
							auto addResult = messageAggregator.modifier().add(pMessage);
							if (addResult < chain::RoundMessageAggregatorAddResult::Neutral_Redundant)
								BITXORCORE_LOG(warning) << "finalization message " << messageHash << " rejected due to " << addResult;
						});
						newMessages.push_back(pMessage);
					}

					if (!newMessages.empty())
						messagesSink(newMessages);
				});
			}

		private:
			FinalizationConfiguration m_config;
		};
	}

	DECLARE_SERVICE_REGISTRAR(FinalizationMessageProcessing)(const FinalizationConfiguration& config) {
		return std::make_unique<FinalizationMessageProcessingServiceRegistrar>(config);
	}
}}
