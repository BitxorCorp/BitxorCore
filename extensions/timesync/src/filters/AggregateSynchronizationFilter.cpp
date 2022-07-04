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

#include "SynchronizationFilters.h"
#include "AggregateSynchronizationFilter.h"
#include "bitxorcore/utils/ContainerHelpers.h"

namespace bitxorcore { namespace timesync { namespace filters {

	AggregateSynchronizationFilter::AggregateSynchronizationFilter(const std::vector<SynchronizationFilter>& filters)
			: m_filters(filters)
	{}

	size_t AggregateSynchronizationFilter::size() const {
		return m_filters.size();
	}

	void AggregateSynchronizationFilter::operator()(TimeSynchronizationSamples& samples, NodeAge nodeAge) {
		utils::map_erase_if(samples, [&filters = m_filters, nodeAge](const auto& sample) {
			return std::any_of(filters.cbegin(), filters.cend(), [&sample, nodeAge](const auto& filter) {
				return filter(sample, nodeAge);
			});
		});

		// alpha trim on both ends
		auto samplesToDiscardAtBothEnds = static_cast<size_t>(static_cast<double>(samples.size()) * Alpha / 2.0);
		for (auto i = 0u; i < samplesToDiscardAtBothEnds; ++i) {
			samples.erase(samples.cbegin());
			samples.erase(--samples.cend());
		}
	}
}}}
