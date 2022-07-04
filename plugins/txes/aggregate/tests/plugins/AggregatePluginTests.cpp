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

#include "src/plugins/AggregatePlugin.h"
#include "src/model/AggregateEntityType.h"
#include "tests/test/plugins/PluginManagerFactory.h"
#include "tests/test/plugins/PluginTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace plugins {

	namespace {
		template<bool EnableStrict, bool EnableBonded>
		struct BasicAggregatePluginTraits : public test::EmptyPluginTraits {
		public:
			template<typename TAction>
			static void RunTestAfterRegistration(TAction action) {
				// Arrange:
				auto config = model::BlockchainConfiguration::Uninitialized();
				config.Plugins.emplace("bitxorcore.plugins.aggregate", utils::ConfigurationBag({{
					"",
					{
						{ "maxTransactionsPerAggregate", "0" },
						{ "maxCosignaturesPerAggregate", "0" },

						{ "enableStrictCosignatureCheck", EnableStrict ? "true" : "false" },
						{ "enableBondedAggregateSupport", EnableBonded ? "true" : "false" },

						{ "maxBondedTransactionLifetime", "1h" }
					}
				}}));

				auto manager = test::CreatePluginManager(config);
				RegisterAggregateSubsystem(manager);

				// Act:
				action(manager);
			}
		};

		// notice that the transaction types and stateless validators are config-dependent

		struct AggregatePluginTraits : public BasicAggregatePluginTraits<false, false> {
		public:
			static std::vector<model::EntityType> GetTransactionTypes() {
				return { model::Entity_Type_Aggregate_Complete };
			}

			static std::vector<std::string> GetStatelessValidatorNames() {
				return { "BasicAggregateCosignaturesValidator" };
			}
		};

		struct StrictAggregatePluginTraits : public BasicAggregatePluginTraits<true, false> {
		public:
			static std::vector<model::EntityType> GetTransactionTypes() {
				return { model::Entity_Type_Aggregate_Complete };
			}

			static std::vector<std::string> GetStatelessValidatorNames() {
				return { "BasicAggregateCosignaturesValidator", "StrictAggregateCosignaturesValidator" };
			}
		};

		struct BondedAggregatePluginTraits : public BasicAggregatePluginTraits<false, true> {
		public:
			static std::vector<model::EntityType> GetTransactionTypes() {
				return { model::Entity_Type_Aggregate_Complete, model::Entity_Type_Aggregate_Bonded };
			}

			static std::vector<std::string> GetStatelessValidatorNames() {
				return { "BasicAggregateCosignaturesValidator" };
			}
		};
	}

	DEFINE_PLUGIN_TESTS(AggregatePluginTests, AggregatePluginTraits)
	DEFINE_PLUGIN_TESTS(StrictAggregatePluginTests, StrictAggregatePluginTraits)
	DEFINE_PLUGIN_TESTS(BondedAggregatePluginTests, BondedAggregatePluginTraits)
}}
