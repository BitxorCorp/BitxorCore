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
#include "bitxorcore/exceptions.h"
#include <list>

namespace bitxorcore { namespace state {

	/// Lock info history.
	template<typename TLockInfo>
	struct LockInfoHistory {
	public:
		static constexpr auto Is_Deactivation_Destructive = true;

	public:
		using ValueType = TLockInfo;
		using LockIdentifier = Hash256;

	public:
		/// Creates a history around \a id.
		explicit LockInfoHistory(const LockIdentifier& id) : m_id(id)
		{}

	public:
		/// Gets the id of the history.
		const LockIdentifier& id() const {
			return m_id;
		}

		/// Gets a value indicating whether or not the history is empty.
		bool empty() const {
			return m_lockInfos.empty();
		}

		/// Gets the history size.
		size_t historyDepth() const {
			return m_lockInfos.size();
		}

	public:
		/// Adds a new lock info (\a lockInfo) at the end of the history.
		void push_back(const ValueType& lockInfo) {
			if (m_id != GetLockIdentifier(lockInfo))
				BITXORCORE_THROW_INVALID_ARGUMENT("cannot add lock info with different identifier to history");

			m_lockInfos.push_back(lockInfo);
		}

		/// Removes the last entry in the history.
		void pop_back() {
			if (empty())
				BITXORCORE_THROW_INVALID_ARGUMENT("cannot pop lock info from empty history");

			m_lockInfos.pop_back();
		}

		/// Gets a const reference to the most recent lock info.
		const ValueType& back() const {
			return m_lockInfos.back();
		}

		/// Gets a reference to the most recent lock info.
		ValueType& back() {
			return m_lockInfos.back();
		}

	public:
		/// Gets a const iterator to the first lock info.
		typename std::list<ValueType>::const_iterator begin() const {
			return m_lockInfos.cbegin();
		}

		/// Gets a const iterator to the element following the last lock info.
		typename std::list<ValueType>::const_iterator end() const {
			return m_lockInfos.cend();
		}

	public:
		/// Returns \c true if history is active at \a height.
		bool isActive(Height height) const {
			return !empty() && back().isActive(height);
		}

	private:
		LockIdentifier m_id;
		std::list<ValueType> m_lockInfos;
	};
}}
