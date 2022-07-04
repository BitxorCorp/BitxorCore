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
#include "bitxorcore/ionet/NodeInteractionResultCode.h"
#include "bitxorcore/thread/FutureUtils.h"

namespace bitxorcore { namespace chain {

	/// Entities synchronizer.
	template<typename TSynchronizerTraits>
	class EntitiesSynchronizer {
	public:
		using RemoteApiType = typename TSynchronizerTraits::RemoteApiType;

	private:
		using NodeInteractionFuture = thread::future<ionet::NodeInteractionResultCode>;

	public:
		/// Creates an entities synchronizer around \a traits.
		explicit EntitiesSynchronizer(TSynchronizerTraits&& traits) : m_traits(std::move(traits))
		{}

	public:
		/// Pulls entities from a remote node using \a api.
		NodeInteractionFuture operator()(const RemoteApiType& api) {
			return m_traits.apiCall(api).then([&traits = m_traits, sourceIdentity = api.remoteIdentity()](auto&& rangeFuture) {
				try {
					auto range = rangeFuture.get();
					if (range.empty())
						return ionet::NodeInteractionResultCode::Neutral;

					BITXORCORE_LOG(debug) << "peer returned " << range.size() << " " << TSynchronizerTraits::Name;
					traits.consume(std::move(range), sourceIdentity);
					return ionet::NodeInteractionResultCode::Success;
				} catch (const bitxorcore_runtime_error& e) {
					BITXORCORE_LOG(warning) << "exception thrown while requesting " << TSynchronizerTraits::Name << ": " << e.what();
					return ionet::NodeInteractionResultCode::Failure;
				}
			});
		}

	private:
		TSynchronizerTraits m_traits;
	};
}}
