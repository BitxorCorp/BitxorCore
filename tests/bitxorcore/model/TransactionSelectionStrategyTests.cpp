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

#include "bitxorcore/model/TransactionSelectionStrategy.h"
#include "tests/test/nodeps/ConfigurationTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

	// region parsing

	TEST(TEST_CLASS, CanParseValidStrategyValue) {
		// Arrange:
		auto assertSuccessfulParse = [](const auto& input, const auto& expectedParsedValue) {
			test::AssertParse(input, expectedParsedValue, [](const auto& str, auto& parsedValue) {
				return TryParseValue(str, parsedValue);
			});
		};

		// Assert:
		assertSuccessfulParse("oldest", TransactionSelectionStrategy::Oldest);
		assertSuccessfulParse("minimize-fee", TransactionSelectionStrategy::Minimize_Fee);
		assertSuccessfulParse("maximize-fee", TransactionSelectionStrategy::Maximize_Fee);
	}

	TEST(TEST_CLASS, CannotParseInvalidStrategyValue) {
		test::AssertEnumParseFailure("minimize", TransactionSelectionStrategy::Oldest, [](const auto& str, auto& parsedValue) {
			return TryParseValue(str, parsedValue);
		});
	}

	// endregion
}}
