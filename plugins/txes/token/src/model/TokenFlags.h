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
#include "bitxorcore/utils/BitwiseEnum.h"
#include <stdint.h>

namespace bitxorcore { namespace model {

	/// Token property flags.
	enum class TokenFlags : uint8_t {
		/// No flags present.
		None = 0x00,

		/// Token supports supply changes even when token owner owns partial supply.
		Supply_Mutable = 0x01,

		/// Token supports transfers between arbitrary accounts.
		/// \note When not set, token can only be transferred to and from token owner.
		Transferable = 0x02,

		/// Token supports custom restrictions configured by token owner.
		Restrictable = 0x04,

		/// Token supports revocation of tokens by creator.
		Revokable = 0x08,

		/// All flags.
		All = 0x0F
	};

	MAKE_BITWISE_ENUM(TokenFlags)
}}
