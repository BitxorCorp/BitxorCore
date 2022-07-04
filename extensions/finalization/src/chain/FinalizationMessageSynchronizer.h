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
#include "finalization/src/handlers/FinalizationHandlerTypes.h"
#include "finalization/src/model/FinalizationRoundRange.h"
#include "bitxorcore/chain/RemoteNodeSynchronizer.h"
#include "bitxorcore/model/FinalizationRound.h"
#include "bitxorcore/model/RangeTypes.h"

namespace bitxorcore { namespace api { class RemoteFinalizationApi; } }

namespace bitxorcore { namespace chain {

	/// Function signature for supplying a finalization round range and short hashes pair.
	using FinalizationMessageSynchronizerFilterSupplier = supplier<std::pair<model::FinalizationRoundRange, model::ShortHashRange>>;

	/// Creates a finalization message synchronizer around a message filter supplier (\a messageFilterSupplier)
	/// and message range consumer (\a messageRangeConsumer).
	RemoteNodeSynchronizer<api::RemoteFinalizationApi> CreateFinalizationMessageSynchronizer(
			const FinalizationMessageSynchronizerFilterSupplier& messageFilterSupplier,
			const handlers::MessageRangeHandler& messageRangeConsumer);
}}
