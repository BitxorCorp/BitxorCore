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
#include <list>
#include <thread>

namespace bitxorcore { namespace thread {

	/// Container of multiple threads based on boost thread_group.
	class ThreadGroup {
	public:
		/// Destroys this thread group.
		~ThreadGroup() {
			join();
		}

	public:
		/// Gets the number of threads.
		size_t size() const {
			return m_threads.size();
		}

	public:
		/// Spawns a new thread around \a func.
		template<typename TFunction>
		void spawn(TFunction func) {
			m_threads.emplace_back(func);
		}

		/// Waits for all threads to complete.
		void join() {
			for (auto& thread : m_threads) {
				if (thread.joinable())
					thread.join();
			}
		}

	private:
		std::list<std::thread> m_threads;
	};
}}
