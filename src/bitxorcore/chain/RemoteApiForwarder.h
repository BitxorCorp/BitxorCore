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
#include "bitxorcore/ionet/NodeInteractionResult.h"
#include "bitxorcore/model/NodeIdentity.h"
#include "bitxorcore/model/TransactionPlugin.h"
#include "bitxorcore/net/PacketIoPicker.h"
#include "bitxorcore/thread/Future.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "bitxorcore/utils/ThrottleLogger.h"
#include "bitxorcore/utils/TimeSpan.h"

namespace bitxorcore { namespace chain {

	/// Simplifies interacting with remote nodes via apis.
	class RemoteApiForwarder {
	public:
		/// Creates a forwarder around a peer selector (\a packetIoPicker) with a connection \a timeout
		/// given a transaction registry (\a transactionRegistry) and a friendly name (\a operationName).
		RemoteApiForwarder(
				net::PacketIoPicker& packetIoPicker,
				const model::TransactionRegistry& transactionRegistry,
				const utils::TimeSpan& timeout,
				const std::string& operationName)
				: m_packetIoPicker(packetIoPicker)
				, m_transactionRegistry(transactionRegistry)
				, m_timeout(timeout)
				, m_operationName(operationName)
		{}

	public:
		/// Picks a random peer and wraps an api around it using \a apiFactory. Finally, passes the api to \a action.
		template<typename TRemoteApiAction, typename TRemoteApiFactory>
		thread::future<ionet::NodeInteractionResult> processSync(TRemoteApiAction action, TRemoteApiFactory apiFactory) const {
			auto packetIoPair = m_packetIoPicker.pickOne(m_timeout);
			if (!packetIoPair) {
				BITXORCORE_LOG_THROTTLE(warning, utils::TimeSpan::FromMinutes(1).millis())
						<< "no packet io available for operation '" << m_operationName << "'";
				return thread::make_ready_future(ionet::NodeInteractionResult());
			}

			// pass in a non-owning pointer to the registry
			auto pRemoteApiUnique = apiFactory(*packetIoPair.io(), packetIoPair.node().identity(), m_transactionRegistry);
			auto pRemoteApi = utils::UniqueToShared(std::move(pRemoteApiUnique));

			// extend the lifetimes of pRemoteApi and packetIoPair until the completion of the action
			// (pRemoteApi is a pointer so that the reference taken by action is valid throughout the entire asynchronous action)
			return action(*pRemoteApi).then([pRemoteApi, packetIoPair, operationName = m_operationName](auto&& resultFuture) {
				auto result = resultFuture.get();
				BITXORCORE_LOG_LEVEL(ionet::NodeInteractionResultCode::Neutral == result ? utils::LogLevel::trace : utils::LogLevel::info)
						<< "completed '" << operationName << "' (" << packetIoPair.node() << ") with result " << result;
				return ionet::NodeInteractionResult(packetIoPair.node().identity(), result);
			});
		}

	private:
		net::PacketIoPicker& m_packetIoPicker;
		const model::TransactionRegistry& m_transactionRegistry;
		utils::TimeSpan m_timeout;
		std::string m_operationName;
	};
}}
