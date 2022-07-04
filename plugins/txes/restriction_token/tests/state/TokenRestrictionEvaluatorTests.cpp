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

#include "src/state/TokenRestrictionEvaluator.h"
#include "src/state/TokenAddressRestriction.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenRestrictionEvaluatorTests

	namespace {
		constexpr auto Sentinel_Removal_Value = TokenAddressRestriction::Sentinel_Removal_Value;

		struct ExpectedEvaluateResults {
			bool Equal;
			bool LessThan;
			bool GreaterThan;
			bool Deleted;
		};

		void RunEvaluateSuccessTest(model::TokenRestrictionType restrictionType, const ExpectedEvaluateResults& expected) {
			EXPECT_EQ(expected.Equal, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 123));

			EXPECT_EQ(expected.LessThan, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 122));
			EXPECT_EQ(expected.LessThan, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 1));

			EXPECT_EQ(expected.GreaterThan, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 124));
			EXPECT_EQ(expected.GreaterThan, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 500));

			EXPECT_EQ(expected.Deleted, EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, Sentinel_Removal_Value));
		}

		void RunEvaluateFailureTest(model::TokenRestrictionType restrictionType) {
			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 123));

			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 122));
			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 1));

			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 124));
			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, 500));

			EXPECT_FALSE(EvaluateTokenRestriction({ TokenId(), 123, restrictionType }, Sentinel_Removal_Value));
		}
	}

	TEST(TEST_CLASS, CanEvaluateRule_EQ) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::EQ, { true, false, false, false });
	}

	TEST(TEST_CLASS, CanEvaluateRule_NE) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::NE, { false, true, true, true });
	}

	TEST(TEST_CLASS, CanEvaluateRule_LT) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::LT, { false, true, false, false });
	}

	TEST(TEST_CLASS, CanEvaluateRule_LE) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::LE, { true, true, false, false });
	}

	TEST(TEST_CLASS, CanEvaluateRule_GT) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::GT, { false, false, true, false });
	}

	TEST(TEST_CLASS, CanEvaluateRule_GE) {
		RunEvaluateSuccessTest(model::TokenRestrictionType::GE, { true, false, true, false });
	}

	TEST(TEST_CLASS, CannotEvaluateRule_NONE) {
		RunEvaluateFailureTest(model::TokenRestrictionType::NONE);
	}

	TEST(TEST_CLASS, CannotEvaluateRule_Other) {
		RunEvaluateFailureTest(static_cast<model::TokenRestrictionType>(128));
	}
}}
