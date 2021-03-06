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

#include "BlockExecutor.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/Block.h"
#include "bitxorcore/observers/EntityObserver.h"

namespace bitxorcore { namespace chain {

	namespace {
		observers::ObserverContext CreateObserverContext(
				const BlockExecutionContext& executionContext,
				Height height,
				observers::NotifyMode mode) {
			return observers::ObserverContext(
					model::NotificationContext(height, executionContext.Resolvers),
					executionContext.State,
					mode);
		}

		void ObserveAll(
				const observers::EntityObserver& observer,
				observers::ObserverContext& context,
				const model::WeakEntityInfos& entityInfos) {
			for (const auto& entityInfo : entityInfos)
				observer.notify(entityInfo, context);
		}
	}

	void ExecuteBlock(const model::BlockElement& blockElement, const BlockExecutionContext& executionContext) {
		model::WeakEntityInfos entityInfos;
		model::ExtractEntityInfos(blockElement, entityInfos);

		auto context = CreateObserverContext(executionContext, blockElement.Block.Height, observers::NotifyMode::Commit);
		ObserveAll(executionContext.Observer, context, entityInfos);
	}

	void RollbackBlock(const model::BlockElement& blockElement, const BlockExecutionContext& executionContext) {
		model::WeakEntityInfos entityInfos;
		model::ExtractEntityInfos(blockElement, entityInfos);
		std::reverse(entityInfos.begin(), entityInfos.end());

		auto context = CreateObserverContext(executionContext, blockElement.Block.Height, observers::NotifyMode::Rollback);
		ObserveAll(executionContext.Observer, context, entityInfos);

		// commit removals
		executionContext.State.Cache.sub<cache::AccountStateCache>().commitRemovals();
	}
}}
