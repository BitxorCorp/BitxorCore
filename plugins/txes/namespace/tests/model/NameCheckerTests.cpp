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

#include "src/model/NameChecker.h"
#include "tests/test/NamespaceTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS NameCheckerTests

	namespace {
		bool IsValidName(const std::string& str) {
			return model::IsValidName(reinterpret_cast<const uint8_t*>(str.data()), str.size());
		}
	}

	TEST(TEST_CLASS, NameIsValidWhenAllCharactersAreAlphaNumeric_Random) {
		// Arrange:
		auto name = test::GenerateValidName(7);

		// Assert:
		EXPECT_TRUE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsInvalidWhenNoCharactersAreAlphaNumeric_Random) {
		// Arrange:
		auto name = test::GenerateValidName(7);
		for (auto& ch : name)
			ch = static_cast<char>(ch ^ 0xFF);

		// Assert:
		EXPECT_FALSE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsInvalidWhenSingleCharactersIsNotAlphaNumeric_Random) {
		// Arrange:
		auto name = test::GenerateValidName(7);
		name[3] = static_cast<char>(name[3] ^ 0xFF);

		// Assert:
		EXPECT_FALSE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsValidWhenAllCharactersAreAlphaNumeric) {
		for (const auto& name : { "a", "be", "cat", "doom" })
			EXPECT_TRUE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsValidWhenItContainsSeparator) {
		for (const auto& name : { "al-ce", "al_ce", "alice-", "alice_" })
			EXPECT_TRUE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsInvalidWhenItStartsWithSeparator) {
		for (const auto& name : { "-alice", "_alice" })
			EXPECT_FALSE(IsValidName(name));
	}

	TEST(TEST_CLASS, NameIsInvalidWhenItContainsInvalidCharacters) {
		for (const auto& name : { "al.ce", "alIce", "al ce", "al@ce", "al#ce" })
			EXPECT_FALSE(IsValidName(name));
	}
}}
