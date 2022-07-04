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
#include "bitxorcore/model/FinalizationRound.h"
#include "bitxorcore/model/HeightHashPair.h"
#include "bitxorcore/model/RangeTypes.h"

namespace bitxorcore { namespace io { class BlockStorageView; } }

namespace bitxorcore { namespace io {

	/// Describes a prevote chain.
	struct PrevoteChainDescriptor {
		/// Prevote round.
		model::FinalizationRound Round;

		/// Block height corresponding to the the first prevote hash.
		bitxorcore::Height Height;

		/// Number of prevote hashes.
		size_t HashesCount;
	};

	/// Storage for prevote chains.
	class PrevoteChainStorage {
	public:
		virtual ~PrevoteChainStorage() = default;

	public:
		/// Returns \c true if backed up chain for \a round contains \a heightHashPair.
		virtual bool contains(const model::FinalizationRound& round, const model::HeightHashPair& heightHashPair) const = 0;

		/// Loads backed up chain for \a round up to \a maxHeight.
		virtual model::BlockRange load(const model::FinalizationRound& round, Height maxHeight) const = 0;

		/// Backs up prevote chain, specified by \a descriptor, stored in \a blockStorageView.
		virtual void save(const BlockStorageView& blockStorageView, const PrevoteChainDescriptor& descriptor) = 0;

		/// Removes backed up chain for \a round.
		virtual void remove(const model::FinalizationRound& round) = 0;
	};
}}
