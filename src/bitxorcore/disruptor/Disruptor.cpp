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

#include "Disruptor.h"
#include "bitxorcore/utils/Functional.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace disruptor {

	// short rationale for lack of locks:
	//  1. m_container is initialized with size, so most operations here don't require locks
	//  2. add is guarded by a lock inside ConsumerDispatcher, which checks if the Disruptor is full
	//  3. markSkipped and isSkipped are guarded by a lock inside DisruptorElement

	Disruptor::Disruptor(size_t disruptorSize, size_t elementTraceInterval)
			: m_elementTraceInterval(elementTraceInterval)
			, m_container(disruptorSize)
			, m_allElementsCount(0)
	{}

	DisruptorElementId Disruptor::add(ConsumerInput&& input, const ProcessingCompleteFunc& processingComplete) {
		auto element = DisruptorElement(std::move(input), ++m_allElementsCount, processingComplete);
		if (IsIntervalElementId(element.id(), m_elementTraceInterval))
			BITXORCORE_LOG(debug) << "disruptor queuing " << element;

		m_container.push_back(std::move(element));
		return element.id();
	}

	void Disruptor::markSkipped(PositionType position, const ConsumerResult& result) {
		m_container[position].markSkipped(position, result);
	}

	bool Disruptor::isSkipped(PositionType position) const {
		return m_container[position].isSkipped();
	}
}}
