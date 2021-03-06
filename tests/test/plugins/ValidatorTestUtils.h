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
#include "bitxorcore/validators/NotificationValidator.h"
#include "bitxorcore/validators/ValidatorContext.h"
#include "tests/test/core/ResolverTestUtils.h"

namespace bitxorcore { namespace test {

	// region CreateValidatorContext

	/// Creates a validator context around \a height, \a network and \a cache.
	inline validators::ValidatorContext CreateValidatorContext(
			Height height,
			const model::NetworkInfo& network,
			const cache::ReadOnlyBitxorCoreCache& cache) {
		return validators::ValidatorContext(model::NotificationContext(height, CreateResolverContextXor()), Timestamp(0), network, cache);
	}

	/// Creates a validator context around \a height and \a cache.
	inline validators::ValidatorContext CreateValidatorContext(Height height, const cache::ReadOnlyBitxorCoreCache& cache) {
		return CreateValidatorContext(height, model::NetworkInfo(), cache);
	}

	// endregion

	// region ValidateNotification

	/// Validates \a notification with \a validator.
	template<typename TNotification>
	validators::ValidationResult ValidateNotification(
			const validators::stateless::NotificationValidatorT<TNotification>& validator,
			const TNotification& notification) {
		return validator.validate(notification);
	}

	/// Validates \a notification with \a validator using \a context.
	template<typename TNotification>
	validators::ValidationResult ValidateNotification(
			const validators::stateful::NotificationValidatorT<TNotification>& validator,
			const TNotification& notification,
			const validators::ValidatorContext& context) {
		return validator.validate(notification, context);
	}

	/// Validates \a notification with \a validator using \a cache at \a height.
	template<typename TNotification>
	validators::ValidationResult ValidateNotification(
			const validators::stateful::NotificationValidatorT<TNotification>& validator,
			const TNotification& notification,
			const cache::BitxorCoreCache& cache,
			Height height = Height(1)) {
		auto cacheView = cache.createView();
		auto readOnlyCache = cacheView.toReadOnly();
		auto context = CreateValidatorContext(height, readOnlyCache);
		return validator.validate(notification, context);
	}

	// endregion

/// Defines common validator tests for a validator with \a NAME.
#define DEFINE_COMMON_VALIDATOR_TESTS(NAME, ...) \
	TEST(NAME##ValidatorTests, CanCreate##NAME##Validator) { \
		auto pValidator = Create##NAME##Validator(__VA_ARGS__); \
		EXPECT_EQ(#NAME "Validator", pValidator->name()); \
	}
}}
