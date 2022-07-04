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

#include "AggregatePlugin.h"
#include "AggregateTransactionPlugin.h"
#include "src/config/AggregateConfiguration.h"
#include "src/model/AggregateEntityType.h"
#include "src/validators/Validators.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	void RegisterAggregateSubsystem(PluginManager& manager) {
		// configure the aggregate to allow all registered transactions that support embedding
		// (this works because the transaction registry is held by reference)
		const auto& transactionRegistry = manager.transactionRegistry();
		auto config = model::LoadPluginConfiguration<config::AggregateConfiguration>(manager.config(), "bitxorcore.plugins.aggregate");
		manager.addTransactionSupport(CreateAggregateTransactionPlugin(transactionRegistry, model::Entity_Type_Aggregate_Complete));
		if (config.EnableBondedAggregateSupport) {
			manager.addTransactionSupport(CreateAggregateTransactionPlugin(
					transactionRegistry,
					config.MaxBondedTransactionLifetime,
					model::Entity_Type_Aggregate_Bonded));
		}

		manager.addStatelessValidatorHook([config](auto& builder) {
			builder.add(validators::CreateBasicAggregateCosignaturesValidator(
					config.MaxTransactionsPerAggregate,
					config.MaxCosignaturesPerAggregate));
			if (config.EnableStrictCosignatureCheck)
				builder.add(validators::CreateStrictAggregateCosignaturesValidator());
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterAggregateSubsystem(manager);
}
