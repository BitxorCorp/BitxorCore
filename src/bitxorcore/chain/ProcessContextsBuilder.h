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
#include "ExecutionConfiguration.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"

namespace bitxorcore {
	namespace cache {
		class BitxorCoreCacheDelta;
		class BitxorCoreCacheView;
	}
	namespace model {
		struct BlockchainConfiguration;
		class BlockStatementBuilder;
	}
	namespace observers { struct ObserverState; }
}

namespace bitxorcore { namespace chain {

	/// Builder for creating process (observer and validator) contexts.
	class ProcessContextsBuilder {
	public:
		/// Creates a builder around \a height, \a blockTime and \a executionContextConfig.
		ProcessContextsBuilder(Height height, Timestamp blockTime, const ExecutionContextConfiguration& executionContextConfig);

	public:
		/// Sets a bitxorcore cache \a view.
		void setCache(const cache::BitxorCoreCacheView& view);

		/// Sets a bitxorcore cache \a delta.
		void setCache(cache::BitxorCoreCacheDelta& delta);

		/// Sets a block statement builder (\a blockStatementBuilder) to use.
		void setBlockStatementBuilder(model::BlockStatementBuilder& blockStatementBuilder);

		/// Sets a bitxorcore observer \a state.
		void setObserverState(const observers::ObserverState& state);

	public:
		/// Builds an observer context.
		observers::ObserverContext buildObserverContext();

		/// Builds a validator context.
		validators::ValidatorContext buildValidatorContext();

	private:
		model::NotificationContext buildNotificationContext();

	private:
		Height m_height;
		Timestamp m_blockTime;
		ExecutionContextConfiguration m_executionContextConfig;

		const cache::BitxorCoreCacheView* m_pCacheView;
		cache::BitxorCoreCacheDelta* m_pCacheDelta;
		std::unique_ptr<cache::ReadOnlyBitxorCoreCache> m_pReadOnlyCache;

		model::BlockStatementBuilder* m_pBlockStatementBuilder;
	};
}}
