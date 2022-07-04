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
#include "Task.h"
#include <string>

namespace bitxorcore { namespace thread { class IoThreadPool; } }

namespace bitxorcore { namespace thread {

	/// Task scheduler.
	class Scheduler {
	public:
		virtual ~Scheduler() = default;

	public:
		/// Gets the number of tasks that are currently either scheduled or executing.
		virtual uint32_t numScheduledTasks() const = 0;

		/// Gets the number of currently executing task callbacks.
		virtual uint32_t numExecutingTaskCallbacks() const = 0;

	public:
		/// Adds a scheduled task to the scheduler.
		virtual void addTask(const Task& task) = 0;

		/// Shuts down the scheduler.
		virtual void shutdown() = 0;
	};

	/// Creates a scheduler around the specified thread \a pool.
	std::shared_ptr<Scheduler> CreateScheduler(IoThreadPool& pool);
}}
