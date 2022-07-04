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
#include "bitxorcore/subscribers/FinalizationSubscriber.h"
#include "tests/test/nodeps/ParamsCapture.h"

namespace bitxorcore { namespace mocks {

	/// Finalization subscriber finalized block params.
	struct FinalizationSubscriberFinalizedBlockParams {
	public:
		/// Creates params around \a round, \a height and \a hash.
		FinalizationSubscriberFinalizedBlockParams(const model::FinalizationRound& round, bitxorcore::Height height, const Hash256& hash)
				: Round(round)
				, Height(height)
				, Hash(hash)
		{}

	public:
		/// Finalization round.
		const model::FinalizationRound Round;

		/// Block height.
		const bitxorcore::Height Height;

		/// Block hash.
		const Hash256 Hash;
	};

	/// Mock finalization subscriber implementation.
	class MockFinalizationSubscriber : public subscribers::FinalizationSubscriber {
	public:
		/// Gets the params passed to notifyFinalizedBlock.
		const auto& finalizedBlockParams() const {
			return m_finalizedBlockParams;
		}

	public:
		void notifyFinalizedBlock(const model::FinalizationRound& round, Height height, const Hash256& hash) override {
			m_finalizedBlockParams.push(round, height, hash);
		}

	private:
		test::ParamsCapture<FinalizationSubscriberFinalizedBlockParams> m_finalizedBlockParams;
	};
}}
