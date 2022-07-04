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
#include "bitxorcore/types.h"

namespace bitxorcore { namespace state {

	/// Lock status.
	enum class LockStatus : uint8_t {
		/// Lock is unused.
		Unused,

		/// Lock was already used.
		Used
	};

	/// Lock info.
	struct LockInfo {
	protected:
		/// Creates a default lock info.
		LockInfo()
		{}

		/// Creates a lock info around \a ownerAddress, \a tokenId, \a amount and \a endHeight.
		LockInfo(const Address& ownerAddress, bitxorcore::TokenId tokenId, bitxorcore::Amount amount, Height endHeight)
				: OwnerAddress(ownerAddress)
				, TokenId(tokenId)
				, Amount(amount)
				, EndHeight(endHeight)
				, Status(LockStatus::Unused)
		{}

	public:
		/// Owner address.
		Address OwnerAddress;

		/// Token id.
		bitxorcore::TokenId TokenId;

		/// Amount.
		bitxorcore::Amount Amount;

		/// Height at which the lock expires.
		Height EndHeight;

		/// Flag indicating whether or not the lock was already used.
		LockStatus Status;

	public:
		/// Returns \c true if lock info is active at \a height.
		constexpr bool isActive(Height height) const {
			return height < EndHeight && LockStatus::Unused == Status;
		}
	};
}}
