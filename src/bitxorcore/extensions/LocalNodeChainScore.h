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
#include "bitxorcore/model/ChainScore.h"
#include "bitxorcore/utils/SpinReaderWriterLock.h"

namespace bitxorcore { namespace extensions {

	/// Chain score stored by the local node.
	/// \note This score is synchronized by a reader writer lock.
	class LocalNodeChainScore {
	public:
		/// Creates a default local node chain score.
		LocalNodeChainScore() = default;

		/// Creates a local node chain score around \a score.
		explicit LocalNodeChainScore(const model::ChainScore& score) : m_score (score)
		{}

	public:
		/// Gets the current chain score.
		model::ChainScore get() const {
			auto readLock = m_lock.acquireReader();
			return m_score;
		}

		/// Sets the current chain score to \a score.
		void set(const model::ChainScore& score) {
			auto writeLock = m_lock.acquireWriter();
			m_score = score;
		}

	public:
		/// Adds \a rhs to this chain score.
		LocalNodeChainScore& operator+=(const model::ChainScore& rhs) {
			auto writeLock = m_lock.acquireWriter();
			m_score += rhs;
			return *this;
		}

		/// Adds \a rhs to this chain score.
		LocalNodeChainScore& operator+=(model::ChainScore::Delta rhs) {
			auto writeLock = m_lock.acquireWriter();
			m_score += rhs;
			return *this;
		}

	private:
		model::ChainScore m_score;
		mutable utils::SpinReaderWriterLock m_lock;
	};
}}
