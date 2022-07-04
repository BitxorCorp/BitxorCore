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
/// Defines a transfer validation result with \a DESCRIPTION and \a CODE.
#define DEFINE_TRANSFER_RESULT(DESCRIPTION, CODE) DEFINE_VALIDATION_RESULT(Failure, Transfer, DESCRIPTION, CODE, None)

	/// Validation failed because the message is too large.
	DEFINE_TRANSFER_RESULT(Message_Too_Large, 1);

	/// Validation failed because tokens are out of order.
	DEFINE_TRANSFER_RESULT(Out_Of_Order_Tokens, 2);

#ifndef CUSTOM_RESULT_DEFINITION
}}
#endif
