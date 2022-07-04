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

#include "tests/bitxorcore/deltaset/test/BaseSetDeltaTests.h"
#include "tests/bitxorcore/deltaset/test/BaseSetTests.h"

namespace bitxorcore { namespace deltaset {

	namespace {
		// to emulate storage virtualization, use two separate sets (ordered and unordered)
		template<typename TMutabilityTraits, typename TElement = test::SetElementType<TMutabilityTraits>>
		using SetVirtualizedTraits = test::BaseSetTraits<
			TMutabilityTraits,
			SetStorageTraits<
				std::set<TElement, std::less<TElement>>,
				std::unordered_set<TElement, test::Hasher<TElement>, test::EqualityChecker<TElement>>
			>
		>;

		using SetVirtualizedMutableTraits = SetVirtualizedTraits<test::MutableElementValueTraits>;
		using SetVirtualizedImmutableTraits = SetVirtualizedTraits<test::ImmutableElementValueTraits>;
	}

// iteration is not supported by virtualized sets
#undef DEFINE_BASE_SET_ITERATION_TESTS
#undef DEFINE_BASE_SET_DELTA_ITERATION_TESTS
#define DEFINE_BASE_SET_ITERATION_TESTS(TEST_CLASS, TRAITS)
#define DEFINE_BASE_SET_DELTA_ITERATION_TESTS(TEST_CLASS, TRAITS)

// base (mutable)
DEFINE_MUTABLE_BASE_SET_TESTS_FOR(SetVirtualizedMutable)

// base (immutable)
DEFINE_IMMUTABLE_BASE_SET_TESTS_FOR(SetVirtualizedImmutable)

// delta (mutable)
DEFINE_MUTABLE_BASE_SET_DELTA_TESTS_FOR(SetVirtualizedMutable)

// delta (immutable)
DEFINE_IMMUTABLE_BASE_SET_DELTA_TESTS_FOR(SetVirtualizedImmutable)
}}
