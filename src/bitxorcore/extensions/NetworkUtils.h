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
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/ionet/NodeInfo.h"
#include "bitxorcore/ionet/RateMonitor.h"
#include "bitxorcore/net/AsyncTcpServer.h"
#include "bitxorcore/net/ConnectionSettings.h"
#include "bitxorcore/thread/MultiServicePool.h"

namespace bitxorcore {
	namespace net { class AcceptedConnectionContainer; }
	namespace subscribers { class NodeSubscriber; }
}

namespace bitxorcore { namespace extensions {

	/// Gets the rate monitor settings from \a banConfig.
	ionet::RateMonitorSettings GetRateMonitorSettings(const config::NodeConfiguration::BanningSubConfiguration& banConfig);

	/// Extracts connection settings from \a config.
	net::ConnectionSettings GetConnectionSettings(const config::BitxorCoreConfiguration& config);

	/// Updates \a settings with values in \a config.
	void UpdateAsyncTcpServerSettings(net::AsyncTcpServerSettings& settings, const config::BitxorCoreConfiguration& config);

	/// Boots a tcp server with \a serviceGroup on localhost \a port with connection \a config and \a acceptor given \a timeSupplier.
	/// Incoming connections are assumed to be associated with \a serviceId and are added to \a nodeSubscriber.
	std::shared_ptr<net::AsyncTcpServer> BootServer(
			thread::MultiServicePool::ServiceGroup& serviceGroup,
			unsigned short port,
			ionet::ServiceIdentifier serviceId,
			const config::BitxorCoreConfiguration& config,
			const supplier<Timestamp>& timeSupplier,
			subscribers::NodeSubscriber& nodeSubscriber,
			net::AcceptedConnectionContainer& acceptor);
}}
