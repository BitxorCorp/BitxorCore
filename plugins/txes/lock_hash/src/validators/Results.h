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

/// Defines a lock hash validation result with \a DESCRIPTION and \a CODE.
#define DEFINE_LOCK_HASH_RESULT(DESCRIPTION, CODE) DEFINE_VALIDATION_RESULT(Failure, LockHash, DESCRIPTION, CODE, None)

	/// Validation failed because lock does not allow the specified token.
	DEFINE_LOCK_HASH_RESULT(Invalid_Token_Id, 1);

	/// Validation failed because lock does not allow the specified amount.
	DEFINE_LOCK_HASH_RESULT(Invalid_Token_Amount, 2);

	/// Validation failed because hash is already present in cache.
	DEFINE_LOCK_HASH_RESULT(Hash_Already_Exists, 3);

	/// Validation failed because hash is not present in cache.
	DEFINE_LOCK_HASH_RESULT(Unknown_Hash, 4);

	/// Validation failed because hash is inactive.
	DEFINE_LOCK_HASH_RESULT(Inactive_Hash, 5);

	/// Validation failed because duration is too long.
	DEFINE_LOCK_HASH_RESULT(Invalid_Duration, 6);

#ifndef CUSTOM_RESULT_DEFINITION
}}
#endif
