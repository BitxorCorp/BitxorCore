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

#include "Signals.h"
#include "bitxorcore/utils/Logging.h"
#include <boost/asio.hpp>

namespace bitxorcore { namespace process {

	namespace {
		struct SignalInfo {
			boost::system::error_code Error;
			int Id;
		};
	}

	void WaitForTerminationSignal() {
		boost::asio::io_context ioContext;
		boost::asio::signal_set signals(ioContext, SIGINT, SIGTERM);

		SignalInfo info;
		signals.async_wait([&info](const auto& ec, auto signalId) {
			info.Error = ec;
			info.Id = signalId;
		});

		BITXORCORE_LOG(info) << "waiting for termination signal";
		ioContext.run();

		if (info.Error)
			BITXORCORE_LOG(warning) << "error waiting for termination signal: " << info.Error;
		else
			BITXORCORE_LOG(info) << "termination signal " << info.Id << " received";
	}
}}
