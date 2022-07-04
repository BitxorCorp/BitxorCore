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

#include "src/validators/Validators.h"
#include "tests/test/plugins/DiscreteIntegerValidatorTests.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenRestrictionTypeValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenRestrictionType,)

	namespace {
		struct TokenRestrictionTypeTraits {
			using EnumType = model::TokenRestrictionType;

			static constexpr auto Failure_Result = Failure_RestrictionToken_Invalid_Restriction_Type;
			static constexpr auto CreateValidator = CreateTokenRestrictionTypeValidator;

			static std::vector<uint8_t> ValidValues() {
				return { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
			}

			static std::vector<uint8_t> InvalidValues() {
				return { 0x07, 0xFF };
			}

			static auto CreateNotification(EnumType value) {
				return model::TokenRestrictionTypeNotification(value);
			}
		};
	}

	DEFINE_DISCRETE_INTEGER_VALIDATOR_TESTS(TEST_CLASS, TokenRestrictionTypeTraits)
}}
