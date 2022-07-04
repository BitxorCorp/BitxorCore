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
#include <type_traits>

namespace bitxorcore { namespace utils {

	/// Applies an accumulator function (\a fun) against an initial value(\a initialValue) and
	/// each element of \a container to reduce it to a single value.
	template<typename TContainer, typename TInitial, typename TFunction>
	auto Reduce(const TContainer& container, TInitial initialValue, TFunction fun) {
		for (const auto& element : container)
			initialValue = fun(initialValue, element);

		return initialValue;
	}

	/// Applies \a accessor on each element of \a container and sums resulting values.
	/// \note this does not use Reduce in order to avoid creating another lambda.
	template<typename TContainer, typename TFunction>
	auto Sum(const TContainer& container, TFunction accessor) {
		using ReturnType = std::invoke_result_t<TFunction, typename TContainer::value_type>;
		auto sum = ReturnType();

		for (const auto& element : container)
			sum = sum + accessor(element);

		return sum;
	}
}}
