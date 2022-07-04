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
#include "ObserverStatementBuilder.h"
#include "bitxorcore/cache/BitxorCoreCacheDelta.h"
#include "bitxorcore/model/NotificationContext.h"
#include "bitxorcore/state/BitxorCoreState.h"
#include <iosfwd>

namespace bitxorcore { namespace observers {

	// region NotifyMode

#define NOTIFY_MODE_LIST \
	/* Execute actions. */ \
	ENUM_VALUE(Commit) \
	\
	/* Reverse actions. */ \
	ENUM_VALUE(Rollback)

#define ENUM_VALUE(LABEL) LABEL,
	/// Enumeration of possible notification modes.
	enum class NotifyMode {
		NOTIFY_MODE_LIST
	};
#undef ENUM_VALUE

	/// Insertion operator for outputting \a value to \a out.
	std::ostream& operator<<(std::ostream& out, NotifyMode value);

	// endregion

	// region ObserverState

	/// Block independent mutable state passed to all observers.
	struct ObserverState {
	public:
		/// Creates an observer state around \a cache.
		explicit ObserverState(cache::BitxorCoreCacheDelta& cache);

		/// Creates an observer state around \a cache and \a blockStatementBuilder.
		ObserverState(cache::BitxorCoreCacheDelta& cache, model::BlockStatementBuilder& blockStatementBuilder);

	public:
		/// BitxorCore cache.
		cache::BitxorCoreCacheDelta& Cache;

		/// Optional block statement builder.
		model::BlockStatementBuilder* pBlockStatementBuilder;
	};

	// endregion

	// region ObserverContext

	/// Context passed to all the observers.
	struct ObserverContext : public model::NotificationContext {
	public:
		/// Creates an observer context around \a notificationContext, \a state and \a mode.
		/// \note \a state is const to enable more consise code even though it only contains non-const references.
		ObserverContext(const model::NotificationContext& notificationContext, const ObserverState& state, NotifyMode mode);

	public:
		/// BitxorCore cache.
		cache::BitxorCoreCacheDelta& Cache;

		/// Notification mode.
		const NotifyMode Mode;

		/// Original (undecorated) alias resolvers from the notification context.
		/// \note These are used during undo to avoid adding resolutions.
		const model::ResolverContext UndecoratedResolvers;

	public:
		/// Statement builder.
		ObserverStatementBuilder& StatementBuilder();

	private:
		ObserverStatementBuilder m_statementBuilder;
	};

	// endregion
}}
