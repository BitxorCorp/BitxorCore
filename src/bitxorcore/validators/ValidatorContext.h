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
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/model/NetworkInfo.h"
#include "bitxorcore/model/NotificationContext.h"
#include "bitxorcore/plugins.h"

namespace bitxorcore { namespace validators {

	/// Contextual information passed to stateful validators.
	struct PLUGIN_API_DEPENDENCY ValidatorContext : public model::NotificationContext {
	public:
		/// Creates a validator context around \a notificationContext, \a blockTime, \a network and \a cache.
		ValidatorContext(
				const model::NotificationContext& notificationContext,
				Timestamp blockTime,
				const model::NetworkInfo& network,
				const cache::ReadOnlyBitxorCoreCache& cache)
				: NotificationContext(notificationContext.Height, notificationContext.Resolvers)
				, BlockTime(blockTime)
				, Network(network)
				, Cache(cache)
		{}

	public:
		/// Current block time.
		const Timestamp BlockTime;

		/// Network info.
		const model::NetworkInfo Network;

		/// BitxorCore cache.
		const cache::ReadOnlyBitxorCoreCache& Cache;
	};
}}
