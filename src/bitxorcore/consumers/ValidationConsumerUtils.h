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
#include "BlockConsumers.h"
#include "bitxorcore/disruptor/DisruptorConsumer.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace consumers {

	// functions in this file are used to implement validation consumers and are tested indirectly through those implementations

	/// Makes a block validation consumer that forwards all entity infos to \a process for validation.
	/// Validation will only be performed for entities for which \a requiresValidationPredicate returns \c true.
	disruptor::ConstBlockConsumer MakeBlockValidationConsumer(
			const RequiresValidationPredicate& requiresValidationPredicate,
			const std::function<validators::ValidationResult (const model::WeakEntityInfos&)>& process);

	/// Makes a transaction validation consumer that forwards all entity infos to \a process for validation.
	/// Each failure is forwarded to \a failedTransactionSink.
	disruptor::TransactionConsumer MakeTransactionValidationConsumer(
			const chain::FailedTransactionSink& failedTransactionSink,
			const std::function<std::vector<validators::ValidationResult> (model::WeakEntityInfos&)>& process);
}}
