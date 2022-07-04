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

#include "src/extensions/TokenUnit.h"
#include "tests/test/nodeps/Comparison.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS TokenUnitTests

	// region constructor

	TEST(TEST_CLASS, CanCreateDefaultUnit) {
		// Arrange:
		TokenUnit<3> unit;

		// Act + Assert:
		EXPECT_EQ(BasicUnitAmount(0), unit.basicUnit());
		EXPECT_EQ(Amount(0), unit.atomicUnit());
		EXPECT_FALSE(unit.isFractional());
	}

	TEST(TEST_CLASS, CanCreateUnitFromAmount) {
		// Arrange:
		TokenUnit<2> unit(Amount(123'000'000));

		// Act + Assert:
		EXPECT_EQ(BasicUnitAmount(1'230'000), unit.basicUnit());
		EXPECT_EQ(Amount(123'000'000), unit.atomicUnit());
		EXPECT_FALSE(unit.isFractional());
	}

	TEST(TEST_CLASS, CanCreateUnitFromBasicUnitAmount) {
		// Arrange:
		TokenUnit<3> unit(BasicUnitAmount(123'000'000));

		// Act + Assert:
		EXPECT_EQ(BasicUnitAmount(123'000'000), unit.basicUnit());
		EXPECT_EQ(Amount(123'000'000'000), unit.atomicUnit());
		EXPECT_FALSE(unit.isFractional());
	}

	TEST(TEST_CLASS, CanCreateUnitFromFractionalAmount) {
		// Arrange:
		TokenUnit<6> unit(Amount(123'789'432));

		// Act + Assert:
		EXPECT_EQ(BasicUnitAmount(123), unit.basicUnit());
		EXPECT_EQ(Amount(123'789'432), unit.atomicUnit());
		EXPECT_TRUE(unit.isFractional());
	}

	// endregion

	// region copy + assign

	TEST(TEST_CLASS, CanCopyAssign) {
		// Arrange:
		TokenUnit<1> unit(BasicUnitAmount(123));
		TokenUnit<1> copy(BasicUnitAmount(456));

		// Act:
		const auto& assignResult = (copy = unit);

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123), unit.basicUnit());
		EXPECT_EQ(BasicUnitAmount(123), copy.basicUnit());
		EXPECT_EQ(&copy, &assignResult);
	}

	TEST(TEST_CLASS, CanCopyConstruct) {
		// Act:
		TokenUnit<1> unit(BasicUnitAmount(123));
		TokenUnit<1> copy(unit);

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123), unit.basicUnit());
		EXPECT_EQ(BasicUnitAmount(123), copy.basicUnit());
	}

	TEST(TEST_CLASS, CanMoveAssign) {
		// Arrange:
		TokenUnit<1> unit(BasicUnitAmount(123));
		TokenUnit<1> copy(BasicUnitAmount(456));

		// Act:
		const auto& assignResult = (copy = std::move(unit));

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123), copy.basicUnit());
		EXPECT_EQ(&copy, &assignResult);
	}

	TEST(TEST_CLASS, CanMoveConstruct) {
		// Act:
		TokenUnit<1> unit(BasicUnitAmount(123));
		TokenUnit<1> copy(std::move(unit));

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123), copy.basicUnit());
	}

	// endregion

	// region custom assign

	TEST(TEST_CLASS, CanAssignUnitFromAmount) {
		// Act:
		TokenUnit<3> unit;
		const auto& assignResult = (unit = Amount(123'000'000));

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123'000), unit.basicUnit());
		EXPECT_EQ(Amount(123'000'000), unit.atomicUnit());
		EXPECT_FALSE(unit.isFractional());
		EXPECT_EQ(&unit, &assignResult);
	}

	TEST(TEST_CLASS, CanAssignUnitFromBasicUnitAmount) {
		// Act:
		TokenUnit<4> unit;
		const auto& assignResult = (unit = BasicUnitAmount(123'000'000));

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123'000'000), unit.basicUnit());
		EXPECT_EQ(Amount(1'230'000'000'000), unit.atomicUnit());
		EXPECT_FALSE(unit.isFractional());
		EXPECT_EQ(&unit, &assignResult);
	}

	TEST(TEST_CLASS, CanAssignUnitFromFractionalAmount) {
		// Arrange:
		TokenUnit<6> unit;
		const auto& assignResult = (unit = Amount(123'789'432));

		// Assert:
		EXPECT_EQ(BasicUnitAmount(123), unit.basicUnit());
		EXPECT_EQ(Amount(123'789'432), unit.atomicUnit());
		EXPECT_TRUE(unit.isFractional());
		EXPECT_EQ(&unit, &assignResult);
	}

	// endregion

	// region comparison operators

	namespace {
		std::vector<TokenUnit<3>> GenerateIncreasingValues() {
			return { TokenUnit<3>(Amount(123)), TokenUnit<3>(Amount(642)), TokenUnit<3>(Amount(989)) };
		}
	}

	DEFINE_EQUALITY_TESTS(TEST_CLASS, GenerateIncreasingValues())

	// endregion

	// region to string

	TEST(TEST_CLASS, CanOutputWholeUnitAmount) {
		// Arrange:
		TokenUnit<9> unit(BasicUnitAmount(123));

		// Act:
		auto str = test::ToString(unit);

		// Assert:
		EXPECT_EQ("123", str);
	}

	TEST(TEST_CLASS, CanOutputFractionalUnitAmount) {
		// Arrange:
		TokenUnit<3> unitDivisibility3(Amount(123'009'032));
		TokenUnit<6> unitDivisibility6(Amount(123'009'032));

		// Act:
		auto strDivisibility3 = test::ToString(unitDivisibility3);
		auto strDivisibility6 = test::ToString(unitDivisibility6);

		// Assert:
		EXPECT_EQ("123009.032", strDivisibility3);
		EXPECT_EQ("123.009032", strDivisibility6);
	}

	TEST(TEST_CLASS, OutputFormattingChangesDoNotLeak) {
		// Arrange:
		std::ostringstream out;
		out.flags(std::ios::hex | std::ios::uppercase);
		out.fill('~');

		// Act:
		out << std::setw(4) << 0xAB << " " << TokenUnit<6>(Amount(123'009'432)) << " " << std::setw(4) << 0xCD;
		auto actual = out.str();

		// Assert:
		EXPECT_EQ("~~AB 123.009432 ~~CD", actual);
	}

	// endregion
}}
