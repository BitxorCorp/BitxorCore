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
#include "bitxorcore/exceptions.h"
#include <vector>

namespace bitxorcore { namespace extensions {

	/// Sets the function \a dest to \a source if and only if \a dest is unset.
	template<typename TFunc>
	void SetOnce(TFunc& dest, const TFunc& source) {
		if (dest)
			BITXORCORE_THROW_INVALID_ARGUMENT("server hook cannot be set more than once");

		dest = source;
	}

	/// Gets the function \a func if and only if it is set.
	template<typename TFunc>
	const TFunc& Require(const TFunc& func) {
		if (!func)
			BITXORCORE_THROW_INVALID_ARGUMENT("server hook has not been set");

		return func;
	}

	/// Aggregates multiple \a consumers into a single consumer.
	template<typename TConsumer>
	TConsumer AggregateConsumers(const std::vector<TConsumer>& consumers) {
		return [consumers](const auto& data) {
			for (const auto& consumer : consumers)
				consumer(data);
		};
	}
}}
