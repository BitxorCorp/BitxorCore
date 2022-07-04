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

#include "ObserverContext.h"
#include "bitxorcore/utils/MacroBasedEnumIncludes.h"

namespace bitxorcore { namespace observers {

	// region NotifyMode

#define DEFINE_ENUM NotifyMode
#define ENUM_LIST NOTIFY_MODE_LIST
#include "bitxorcore/utils/MacroBasedEnum.h"
#undef ENUM_LIST
#undef DEFINE_ENUM

	// endregion

	// region ObserverState

	ObserverState::ObserverState(cache::BitxorCoreCacheDelta& cache)
			: Cache(cache)
			, pBlockStatementBuilder(nullptr)
	{}

	ObserverState::ObserverState(cache::BitxorCoreCacheDelta& cache, model::BlockStatementBuilder& blockStatementBuilder)
			: Cache(cache)
			, pBlockStatementBuilder(&blockStatementBuilder)
	{}

	// endregion

	// region ObserverContext

	namespace {
		model::ResolverContext BindConditional(
				const model::ResolverContext& resolvers,
				model::BlockStatementBuilder* pBlockStatementBuilder) {
			return pBlockStatementBuilder ? Bind(resolvers, *pBlockStatementBuilder) : resolvers;
		}

		ObserverStatementBuilder CreateObserverStatementBuilder(model::BlockStatementBuilder* pBlockStatementBuilder) {
			return pBlockStatementBuilder ? ObserverStatementBuilder(*pBlockStatementBuilder) : ObserverStatementBuilder();
		}
	}

	ObserverContext::ObserverContext(const model::NotificationContext& notificationContext, const ObserverState& state, NotifyMode mode)
			: NotificationContext(notificationContext.Height, BindConditional(notificationContext.Resolvers, state.pBlockStatementBuilder))
			, Cache(state.Cache)
			, Mode(mode)
			, UndecoratedResolvers(notificationContext.Resolvers)
			, m_statementBuilder(CreateObserverStatementBuilder(state.pBlockStatementBuilder))
	{}

	ObserverStatementBuilder& ObserverContext::StatementBuilder() {
		return m_statementBuilder;
	}

	// endregion
}}
