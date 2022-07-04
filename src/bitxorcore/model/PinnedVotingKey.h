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
#include <iosfwd>
#include <vector>

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Pinned voting key.
	struct PinnedVotingKey {
	public:
		static constexpr auto Size = bitxorcore::VotingKey::Size + 2 * sizeof(FinalizationEpoch);

	public:
		/// Voting key.
		bitxorcore::VotingKey VotingKey;

		/// Start finalization epoch.
		FinalizationEpoch StartEpoch;

		/// End finalization epoch.
		FinalizationEpoch EndEpoch;

	public:
		/// Returns \c true if this root voting key is equal to \a rhs.
		bool operator==(const PinnedVotingKey& rhs) const {
			return VotingKey == rhs.VotingKey && StartEpoch == rhs.StartEpoch && EndEpoch == rhs.EndEpoch;
		}

		/// Returns \c true if this root voting key is not equal to \a rhs.
		bool operator!=(const PinnedVotingKey& rhs) const {
			return !(*this == rhs);
		}

		/// Insertion operator for outputting \a key to \a out.
		friend std::ostream& operator<<(std::ostream& out, const PinnedVotingKey& key) {
			out << key.VotingKey << " @ [" << key.StartEpoch << ", " << key.EndEpoch << "]";
			return out;
		}
	};

#pragma pack(pop)

	/// Finds the voting public key in \a pinnedPublicKeys that is active at \a epoch.
	/// \note Zero key will be returned when no matching voting public key is found.
	VotingKey FindVotingPublicKeyForEpoch(const std::vector<PinnedVotingKey>& pinnedPublicKeys, FinalizationEpoch epoch);
}}
