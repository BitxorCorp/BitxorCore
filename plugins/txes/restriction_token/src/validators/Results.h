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
#ifndef CUSTOM_RESULT_DEFINITION
#include "bitxorcore/validators/ValidationResult.h"

namespace bitxorcore { namespace validators {

#endif

/// Defines a restriction token validation result with \a DESCRIPTION and \a CODE.
#define DEFINE_RESTRICTION_TOKEN_RESULT(DESCRIPTION, CODE) DEFINE_VALIDATION_RESULT(Failure, RestrictionToken, DESCRIPTION, CODE, None)

	/// Validation failed because the token restriction type is invalid.
	DEFINE_RESTRICTION_TOKEN_RESULT(Invalid_Restriction_Type, 1);

	/// Validation failed because specified previous value does not match current value.
	DEFINE_RESTRICTION_TOKEN_RESULT(Previous_Value_Mismatch, 2);

	/// Validation failed because specified previous value is nonzero.
	DEFINE_RESTRICTION_TOKEN_RESULT(Previous_Value_Must_Be_Zero, 3);

	/// Validation failed because the maximum number of restrictions would be exeeded.
	DEFINE_RESTRICTION_TOKEN_RESULT(Max_Restrictions_Exceeded, 4);

	/// Validation failed because nonexistent restriction cannot be deleted.
	DEFINE_RESTRICTION_TOKEN_RESULT(Cannot_Delete_Nonexistent_Restriction, 5);

	/// Validation failed because required global restriction does not exist.
	DEFINE_RESTRICTION_TOKEN_RESULT(Unknown_Global_Restriction, 6);

	/// Validation failed because token has invalid global restriction.
	DEFINE_RESTRICTION_TOKEN_RESULT(Invalid_Global_Restriction, 7);

	/// Validation failed because account lacks proper permissions to move token.
	DEFINE_RESTRICTION_TOKEN_RESULT(Account_Unauthorized, 8);

#ifndef CUSTOM_RESULT_DEFINITION
}}
#endif
