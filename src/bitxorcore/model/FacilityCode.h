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
#include <stdint.h>

namespace bitxorcore { namespace model {

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow" // FacilityCode::Token shadows model::Token
#endif

	/// Possible facility codes.
	enum class FacilityCode : uint8_t {
		/// Account link facility code.
		AccountLink = 0x4C,

		/// Aggregate facility code.
		Aggregate = 0x41,

		/// Core facility code.
		Core = 0x43,

		/// Lock hash facility code.
		LockHash = 0x48,

		/// Lock secret facility code.
		LockSecret = 0x52,

		/// Metadata facility code.
		Metadata = 0x44,

		/// Token facility code.
		Token = 0x4D,

		/// Multisig facility code.
		Multisig = 0x55,

		/// Namespace facility code.
		Namespace = 0x4E,

		/// Account restriction facility code.
		RestrictionAccount = 0x50,

		/// Token restriction facility code.
		RestrictionToken = 0x51,

		/// Transfer facility code.
		Transfer = 0x54
	};

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
}}
