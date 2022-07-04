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
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/observers/ObserverContext.h"
#include "bitxorcore/state/AccountState.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/ResolverTestUtils.h"

namespace bitxorcore { namespace test {

	/// Observer test context that wraps an observer context.
	template<typename TCacheFactory>
	class ObserverTestContextT {
	public:
		/// Creates a test context around \a mode.
		explicit ObserverTestContextT(observers::NotifyMode mode) : ObserverTestContextT(mode, Height(444))
		{}

		/// Creates a test context around \a mode and \a height.
		ObserverTestContextT(observers::NotifyMode mode, Height height)
				: ObserverTestContextT(mode, height, model::BlockchainConfiguration::Uninitialized())
		{}

		/// Creates a test context around \a mode, \a height and \a config.
		ObserverTestContextT(observers::NotifyMode mode, Height height, const model::BlockchainConfiguration& config)
				: m_cache(TCacheFactory::Create(config))
				, m_cacheDelta(m_cache.createDelta())
				, m_context(
						model::NotificationContext(height, CreateResolverContextXor()),
						observers::ObserverState(m_cacheDelta, m_blockStatementBuilder),
						mode)
		{}

	public:
		/// Gets the observer context.
		observers::ObserverContext& observerContext() {
			return m_context;
		}

		/// Gets the bitxorcore cache delta.
		const cache::BitxorCoreCacheDelta& cache() const {
			return m_cacheDelta;
		}

		/// Gets the bitxorcore cache delta.
		cache::BitxorCoreCacheDelta& cache() {
			return m_cacheDelta;
		}

		/// Gets the bitxorcore state.
		const state::BitxorCoreState& state() const {
			return m_cacheDelta.dependentState();
		}

		/// Gets the bitxorcore state.
		state::BitxorCoreState& state() {
			return m_cacheDelta.dependentState();
		}

		/// Gets the block statement builder.
		model::BlockStatementBuilder& statementBuilder() {
			return m_blockStatementBuilder;
		}

	public:
		/// Commits all changes to the underlying cache.
		void commitCacheChanges() {
			m_cache.commit(Height());
		}

	private:
		cache::BitxorCoreCache m_cache;
		cache::BitxorCoreCacheDelta m_cacheDelta;
		model::BlockStatementBuilder m_blockStatementBuilder;

		observers::ObserverContext m_context;
	};

	/// Default observer test context that wraps a cache composed of core caches only.
	class ObserverTestContext : public ObserverTestContextT<CoreSystemCacheFactory> {
		using ObserverTestContextT::ObserverTestContextT;
	};
}}
