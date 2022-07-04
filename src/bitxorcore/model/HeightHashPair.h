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
#include "EntityRange.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace model {

	/// Height hash pair.
	struct HeightHashPair {
	public:
		static constexpr auto Size = sizeof(bitxorcore::Height) + Hash256::Size;

	public:
		/// Height.
		bitxorcore::Height Height;

		/// Hash.
		Hash256 Hash;

	public:
		/// Returns \c true if this pair is equal to \a rhs.
		constexpr bool operator==(const HeightHashPair& rhs) const {
			return Height == rhs.Height && Hash == rhs.Hash;
		}

		/// Returns \c true if this pair is not equal to \a rhs.
		constexpr bool operator!=(const HeightHashPair& rhs) const {
			return !(*this == rhs);
		}

		/// Insertion operator for outputting \a pair to \a out.
		friend std::ostream& operator<<(std::ostream& out, const HeightHashPair& pair) {
			out << pair.Hash << " @ " << pair.Height;
			return out;
		}
	};
}}
