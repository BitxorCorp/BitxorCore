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

#include "FinalizationMessageSynchronizer.h"
#include "finalization/src/api/RemoteFinalizationApi.h"
#include "bitxorcore/chain/EntitiesSynchronizer.h"
#include "bitxorcore/model/NodeIdentity.h"

namespace bitxorcore { namespace chain {

	namespace {
		struct FinalizationMessageTraits {
		public:
			using RemoteApiType = api::RemoteFinalizationApi;
			static constexpr auto Name = "finalization messages";

		public:
			FinalizationMessageTraits(
					const FinalizationMessageSynchronizerFilterSupplier& messageFilterSupplier,
					const handlers::MessageRangeHandler& messageRangeConsumer)
					: m_messageFilterSupplier(messageFilterSupplier)
					, m_messageRangeConsumer(messageRangeConsumer)
			{}

		public:
			thread::future<model::FinalizationMessageRange> apiCall(const RemoteApiType& api) const {
				auto filter = m_messageFilterSupplier();
				return api.messages(filter.first, std::move(filter.second));
			}

			void consume(model::FinalizationMessageRange&& range, const model::NodeIdentity& sourceIdentity) const {
				m_messageRangeConsumer(model::AnnotatedEntityRange<model::FinalizationMessage>(std::move(range), sourceIdentity));
			}

		private:
			FinalizationMessageSynchronizerFilterSupplier m_messageFilterSupplier;
			handlers::MessageRangeHandler m_messageRangeConsumer;
		};
	}

	RemoteNodeSynchronizer<api::RemoteFinalizationApi> CreateFinalizationMessageSynchronizer(
			const FinalizationMessageSynchronizerFilterSupplier& messageFilterSupplier,
			const handlers::MessageRangeHandler& messageRangeConsumer) {
		auto traits = FinalizationMessageTraits(messageFilterSupplier, messageRangeConsumer);
		auto pSynchronizer = std::make_shared<EntitiesSynchronizer<FinalizationMessageTraits>>(std::move(traits));
		return CreateRemoteNodeSynchronizer(pSynchronizer);
	}
}}
