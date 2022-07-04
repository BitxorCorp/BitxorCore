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
#include "RoundMessageAggregatorAddResult.h"
#include "bitxorcore/model/RangeTypes.h"
#include <memory>
#include <vector>

namespace bitxorcore {
	namespace chain { class RoundContext; }
	namespace model {
		class FinalizationContext;
		struct FinalizationMessage;
	}
}

namespace bitxorcore { namespace chain {

	/// Aggregates finalization messages for a single finalization point.
	class RoundMessageAggregator {
	public:
		using UnknownMessages = std::vector<std::shared_ptr<const model::FinalizationMessage>>;

	public:
		virtual ~RoundMessageAggregator() = default;

	public:
		/// Gets the number of messages.
		virtual size_t size() const = 0;

		/// Gets the finalization context.
		virtual const model::FinalizationContext& finalizationContext() const = 0;

		/// Gets the round context.
		virtual const RoundContext& roundContext() const = 0;

		/// Gets a range of short hashes of all messages in the cache.
		/// \note Each short hash consists of the first 4 bytes of the complete hash.
		virtual model::ShortHashRange shortHashes() const = 0;

		/// Gets all finalization messages that do not have a short hash in \a knownShortHashes.
		virtual UnknownMessages unknownMessages(const utils::ShortHashesSet& knownShortHashes) const = 0;

	public:
		/// Adds a finalization message (\a pMessage) to the aggregator.
		/// \note Message is a shared_ptr because it is detached from an EntityRange and is kept alive with its associated step.
		virtual RoundMessageAggregatorAddResult add(const std::shared_ptr<model::FinalizationMessage>& pMessage) = 0;
	};

	/// Creates a round message aggregator around \a finalizationContext.
	std::unique_ptr<RoundMessageAggregator> CreateRoundMessageAggregator(const model::FinalizationContext& finalizationContext);
}}
