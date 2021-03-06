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

#include "src/plugins/MemoryHashCacheSystem.h"
#include "tests/test/plugins/PluginManagerFactory.h"
#include "tests/test/plugins/PluginTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace plugins {

	namespace {
		struct MemoryHashCacheSystemTraits : public test::EmptyPluginTraits {
		public:
			template<typename TAction>
			static void RunTestAfterRegistration(TAction action) {
				// Arrange:
				auto manager = test::CreatePluginManager();
				RegisterMemoryHashCacheSystem(manager);

				// Act:
				action(manager);
			}

		public:
			static std::vector<std::string> GetCacheNames() {
				return { "HashCache" };
			}

			static std::vector<ionet::PacketType> GetDiagnosticPacketTypes() {
				return { ionet::PacketType::Confirm_Timestamped_Hashes };
			}

			static std::vector<std::string> GetDiagnosticCounterNames() {
				return { "HASH C" };
			}

			static std::vector<std::string> GetStatefulValidatorNames() {
				return { "UniqueTransactionHashValidator" };
			}

			static std::vector<std::string> GetObserverNames() {
				return { "TransactionHashObserver" };
			}
		};
	}

	DEFINE_PLUGIN_TESTS(MemoryHashCacheSystemTests, MemoryHashCacheSystemTraits)
}}
