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
#include "bitxorcore/utils/TimeSpan.h"

namespace bitxorcore { namespace chain { class MultiRoundMessageAggregator; } }

namespace bitxorcore { namespace chain {

	/// Advances through finalization stages.
	class FinalizationStageAdvancer {
	public:
		virtual ~FinalizationStageAdvancer() = default;

	public:
		/// Returns \c true if a prevote can be sent at the specified \a time.
		virtual bool canSendPrevote(Timestamp time) const = 0;

		/// Returns \c true if a precommit can be sent at the specified \a time.
		/// On success, \a target is set to the value to precommit.
		virtual bool canSendPrecommit(Timestamp time, model::HeightHashPair& target) const = 0;

		/// Returns \c true if the next round can be started at the specified \a time.
		virtual bool canStartNextRound() const = 0;
	};

	/// Creates an advancer for the specified \a round given the current \a time, \a stepDuration and \a messageAggregator.
	std::unique_ptr<FinalizationStageAdvancer> CreateFinalizationStageAdvancer(
			const model::FinalizationRound& round,
			Timestamp time,
			const utils::TimeSpan& stepDuration,
			const MultiRoundMessageAggregator& messageAggregator);
}}
