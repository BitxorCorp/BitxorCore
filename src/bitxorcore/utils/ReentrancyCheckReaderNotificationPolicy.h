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
#include "SpinLock.h"
#include "bitxorcore/exceptions.h"
#include <thread>
#include <unordered_set>

namespace bitxorcore { namespace utils {

	/// Exception class that is thrown when reader reentrancy is detected.
	class reader_reentrancy_error : public bitxorcore_runtime_error {
	public:
		using bitxorcore_runtime_error::bitxorcore_runtime_error;
	};

	/// Reentrancy check reader notification policy.
	class ReentrancyCheckReaderNotificationPolicy {
	public:
		/// Reader was acquried by the current thread.
		void readerAcquired() {
			executeSynchronized([this](auto id) {
				if (m_threadIds.cend() != m_threadIds.find(id))
					BITXORCORE_THROW_AND_LOG_1(reader_reentrancy_error, "reader reentrancy detected", id);

				m_threadIds.insert(id);
			});
		}

		/// Reader was released by the current thread.
		void readerReleased() {
			executeSynchronized([this](auto id) {
				m_threadIds.erase(id);
			});
		}

	private:
		template<typename TAction>
		void executeSynchronized(TAction action) {
			auto id = std::this_thread::get_id();
			SpinLockGuard lock(m_mutex);

			action(id);
		}

	private:
		SpinLock m_mutex;
		std::unordered_set<std::thread::id> m_threadIds;
	};
}}
