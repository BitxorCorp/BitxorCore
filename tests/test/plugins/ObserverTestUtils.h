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
#include "ObserverTestContext.h"
#include "bitxorcore/observers/NotificationObserver.h"
#include "tests/test/core/ResolverTestUtils.h"

namespace bitxorcore { namespace test {

	// region CreateObserverContext

	/// Creates an observer context around \a state at \a height with specified \a mode.
	inline observers::ObserverContext CreateObserverContext(
			const observers::ObserverState& state,
			Height height,
			observers::NotifyMode mode) {
		return observers::ObserverContext(model::NotificationContext(height, CreateResolverContextXor()), state, mode);
	}

	/// Creates an observer context around \a cache at \a height with specified \a mode.
	inline observers::ObserverContext CreateObserverContext(cache::BitxorCoreCacheDelta& cache, Height height, observers::NotifyMode mode) {
		return CreateObserverContext(observers::ObserverState(cache), height, mode);
	}

	// endregion

	// region ObserveNotification

	/// Observes \a notification with \a observer using \a context.
	template<typename TNotification>
	void ObserveNotification(
			const observers::NotificationObserverT<TNotification>& observer,
			const TNotification& notification,
			observers::ObserverContext& context) {
		observer.notify(notification, context);
	}

	/// Observes \a notification with \a observer using \a context.
	template<typename TNotification, typename TCacheFactory>
	void ObserveNotification(
			const observers::NotificationObserverT<TNotification>& observer,
			const TNotification& notification,
			ObserverTestContextT<TCacheFactory>& context) {
		ObserveNotification(observer, notification, context.observerContext());
	}

	// endregion

/// Defines common observer tests for an observer with \a NAME.
#define DEFINE_COMMON_OBSERVER_TESTS(NAME, ...) \
	TEST(NAME##ObserverTests, CanCreate##NAME##Observer) { \
		auto pObserver = Create##NAME##Observer(__VA_ARGS__); \
		EXPECT_EQ(#NAME "Observer", pObserver->name()); \
	}
}}
