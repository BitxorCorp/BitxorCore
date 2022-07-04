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

#include "src/plugins/HashLockPlugin.h"
#include "src/model/HashLockEntityType.h"
#include "tests/test/plugins/PluginManagerFactory.h"
#include "tests/test/plugins/PluginTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace plugins {

	namespace {
		struct HashLockPluginTraits {
		public:
			template<typename TAction>
			static void RunTestAfterRegistration(TAction action) {
				// Arrange:
				auto config = model::BlockchainConfiguration::Uninitialized();
				config.BlockGenerationTargetTime = utils::TimeSpan::FromSeconds(1);
				config.Plugins.emplace("bitxorcore.plugins.lockhash", utils::ConfigurationBag({{
					"",
					{
						{ "lockedFundsPerAggregate", "10'000'000" },
						{ "maxHashLockDuration", "2d" }
					}
				}}));

				auto manager = test::CreatePluginManager(config);
				RegisterHashLockSubsystem(manager);

				// Act:
				action(manager);
			}

		public:
			static std::vector<model::EntityType> GetTransactionTypes() {
				return { model::Entity_Type_Hash_Lock };
			}

			static std::vector<std::string> GetCacheNames() {
				return { "HashLockInfoCache" };
			}

			static std::vector<ionet::PacketType> GetNonDiagnosticPacketTypes() {
				return { ionet::PacketType::Hash_Lock_State_Path };
			}

			static std::vector<ionet::PacketType> GetDiagnosticPacketTypes() {
				return { ionet::PacketType::Hash_Lock_Infos };
			}

			static std::vector<std::string> GetDiagnosticCounterNames() {
				return { "HASHLOCK C" };
			}

			static std::vector<std::string> GetStatelessValidatorNames() {
				return { "HashLockDurationValidator" };
			}

			static std::vector<std::string> GetStatefulValidatorNames() {
				return { "AggregateHashPresentValidator", "HashLockCacheUniqueValidator", "HashLockTokenValidator" };
			}

			static std::vector<std::string> GetObserverNames() {
				return {
					"HashLockObserver",
					"ExpiredHashLockInfoObserver",
					"CompletedAggregateObserver"
				};
			}

			static std::vector<std::string> GetPermanentObserverNames() {
				return GetObserverNames();
			}
		};
	}

	DEFINE_PLUGIN_TESTS(HashLockPluginTests, HashLockPluginTraits)
}}
