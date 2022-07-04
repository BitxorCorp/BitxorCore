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

#include "TransactionSinkService.h"
#include "bitxorcore/extensions/ServerHooksUtils.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/handlers/TransactionHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace transactionsink {

	namespace {
		class TransactionSinkServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "TransactionSink", extensions::ServiceRegistrarPhase::Post_Range_Consumers };
			}

			void registerServiceCounters(extensions::ServiceLocator&) override {
				// no additional counters
			}

			void registerServices(extensions::ServiceLocator&, extensions::ServiceState& state) override {
				// add handlers
				auto pushTransactionsCallback = CreateTransactionPushEntityCallback(state.hooks());
				handlers::RegisterPushTransactionsHandler(
						state.packetHandlers(),
						state.pluginManager().transactionRegistry(),
						pushTransactionsCallback);
			}
		};
	}

	DECLARE_SERVICE_REGISTRAR(TransactionSink)() {
		return std::make_unique<TransactionSinkServiceRegistrar>();
	}
}}
