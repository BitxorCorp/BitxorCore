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
/// Defines a token validation result with \a DESCRIPTION and \a CODE.
#define DEFINE_TOKEN_RESULT(DESCRIPTION, CODE) DEFINE_VALIDATION_RESULT(Failure, Token, DESCRIPTION, CODE, None)

	// region common

	/// Validation failed because the duration has an invalid value.
	DEFINE_TOKEN_RESULT(Invalid_Duration, 1);

	/// Validation failed because the name is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Name, 2);

	/// Validation failed because the name and id don't match.
	DEFINE_TOKEN_RESULT(Name_Id_Mismatch, 3);

	/// Validation failed because the parent is expired.
	DEFINE_TOKEN_RESULT(Expired, 4);

	/// Validation failed because the parent owner conflicts with the child owner.
	DEFINE_TOKEN_RESULT(Owner_Conflict, 5);

	/// Validation failed because the id is not the expected id generated from signer and nonce.
	DEFINE_TOKEN_RESULT(Id_Mismatch, 6);

	// endregion

	// region token

	/// Validation failed because the existing parent id does not match the supplied parent id.
	DEFINE_TOKEN_RESULT(Parent_Id_Conflict, 100);

	/// Validation failed because a token property is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Property, 101);

	/// Validation failed because the token flags are invalid.
	DEFINE_TOKEN_RESULT(Invalid_Flags, 102);

	/// Validation failed because the token divisibility is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Divisibility, 103);

	/// Validation failed because the token supply change action is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Supply_Change_Action, 104);

	/// Validation failed because the token supply change amount is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Supply_Change_Amount, 105);

	/// Validation failed because the token id is invalid.
	DEFINE_TOKEN_RESULT(Invalid_Id, 106);

	/// Validation failed because token modification is not allowed.
	DEFINE_TOKEN_RESULT(Modification_Disallowed, 107);

	/// Validation failed because token modification would not result in any changes.
	DEFINE_TOKEN_RESULT(Modification_No_Changes, 108);

	/// Validation failed because the token supply is immutable.
	DEFINE_TOKEN_RESULT(Supply_Immutable, 109);

	/// Validation failed because the resulting token supply is negative.
	DEFINE_TOKEN_RESULT(Supply_Negative, 110);

	/// Validation failed because the resulting token supply exceeds the maximum allowed value.
	DEFINE_TOKEN_RESULT(Supply_Exceeded, 111);

	/// Validation failed because the token is not transferable.
	DEFINE_TOKEN_RESULT(Non_Transferable, 112);

	/// Validation failed because the credit of the token would exceed the maximum of different tokens an account is allowed to own.
	DEFINE_TOKEN_RESULT(Max_Tokens_Exceeded, 113);

	/// Validation failed because the token has at least one required property flag unset.
	DEFINE_TOKEN_RESULT(Required_Property_Flag_Unset, 114);

	// endregion

#ifndef CUSTOM_RESULT_DEFINITION
}}
#endif
