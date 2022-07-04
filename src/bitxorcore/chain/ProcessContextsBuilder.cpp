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

#include "ProcessContextsBuilder.h"
#include "bitxorcore/cache/BitxorCoreCacheDelta.h"
#include "bitxorcore/cache/BitxorCoreCacheView.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/observers/ObserverContext.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace chain {

	ProcessContextsBuilder::ProcessContextsBuilder(
			Height height,
			Timestamp blockTime,
			const ExecutionContextConfiguration& executionContextConfig)
			: m_height(height)
			, m_blockTime(blockTime)
			, m_executionContextConfig(executionContextConfig)
			, m_pCacheView(nullptr)
			, m_pCacheDelta(nullptr)
			, m_pBlockStatementBuilder(nullptr)
	{}

	void ProcessContextsBuilder::setCache(const cache::BitxorCoreCacheView& view) {
		m_pCacheView = &view;
		m_pReadOnlyCache = std::make_unique<cache::ReadOnlyBitxorCoreCache>(view.toReadOnly());
	}

	void ProcessContextsBuilder::setCache(cache::BitxorCoreCacheDelta& delta) {
		m_pCacheDelta = &delta;
		m_pReadOnlyCache = std::make_unique<cache::ReadOnlyBitxorCoreCache>(delta.toReadOnly());
	}

	void ProcessContextsBuilder::setBlockStatementBuilder(model::BlockStatementBuilder& blockStatementBuilder) {
		m_pBlockStatementBuilder = &blockStatementBuilder;
	}

	void ProcessContextsBuilder::setObserverState(const observers::ObserverState& state) {
		setCache(state.Cache);
		m_pBlockStatementBuilder = state.pBlockStatementBuilder;
	}

	observers::ObserverContext ProcessContextsBuilder::buildObserverContext() {
		if (!m_pCacheDelta)
			BITXORCORE_THROW_INVALID_ARGUMENT("buildObserverContext requires BitxorCoreCacheDelta");

		auto observerState = m_pBlockStatementBuilder
				? observers::ObserverState(*m_pCacheDelta, *m_pBlockStatementBuilder)
				: observers::ObserverState(*m_pCacheDelta);
		return observers::ObserverContext(buildNotificationContext(), observerState, observers::NotifyMode::Commit);
	}

	validators::ValidatorContext ProcessContextsBuilder::buildValidatorContext() {
		if (!m_pReadOnlyCache)
			BITXORCORE_THROW_INVALID_ARGUMENT("buildValidatorContext requires BitxorCoreCacheView or BitxorCoreCacheDelta");

		return validators::ValidatorContext(buildNotificationContext(), m_blockTime, m_executionContextConfig.Network, *m_pReadOnlyCache);
	}

	model::NotificationContext ProcessContextsBuilder::buildNotificationContext() {
		auto resolverContext = m_executionContextConfig.ResolverContextFactory(*m_pReadOnlyCache);
		return model::NotificationContext(m_height, resolverContext);
	}
}}
