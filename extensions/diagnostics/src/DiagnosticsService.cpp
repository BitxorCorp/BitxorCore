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

#include "DiagnosticsService.h"
#include "bitxorcore/extensions/ServiceLocator.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/handlers/DiagnosticHandlers.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace diagnostics {

	namespace {
		thread::Task CreateLoggingTask(const std::vector<utils::DiagnosticCounter>& counters) {
			return thread::CreateNamedTask("logging task", [counters]() {
				std::ostringstream table;
				table << "--- current counter values ---";
				for (const auto& counter : counters) {
					table.width(utils::DiagnosticCounterId::Max_Counter_Name_Size);
					table << std::endl << counter.id().name() << " : " << counter.value();
				}

				BITXORCORE_LOG(info) << table.str();
				return thread::make_ready_future(thread::TaskResult::Continue);
			});
		}

		void AddDiagnosticHandlers(const std::vector<utils::DiagnosticCounter>& counters, extensions::ServiceState& state) {
			auto& handlers = state.packetHandlers();
			handlers.setAllowedHosts(state.config().Node.TrustedHosts);

			handlers::RegisterDiagnosticCountersHandler(handlers, counters);
			handlers::RegisterDiagnosticNodesHandler(handlers, state.nodes());
			handlers::RegisterDiagnosticBlockStatementHandler(handlers, state.storage());
			state.pluginManager().addDiagnosticHandlers(handlers, state.cache());

			handlers.setAllowedHosts({});
		}

		class DiagnosticsServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "Diagnostics", extensions::ServiceRegistrarPhase::Initial };
			}

			void registerServiceCounters(extensions::ServiceLocator&) override {
				// no additional counters
			}

			void registerServices(extensions::ServiceLocator& locator, extensions::ServiceState& state) override {
				// merge all counters
				auto counters = state.counters();
				counters.insert(counters.end(), locator.counters().cbegin(), locator.counters().cend());

				// add task
				state.tasks().push_back(CreateLoggingTask(counters));

				// add packet handlers
				AddDiagnosticHandlers(counters, state);
			}
		};
	}

	DECLARE_SERVICE_REGISTRAR(Diagnostics)() {
		return std::make_unique<DiagnosticsServiceRegistrar>();
	}
}}
