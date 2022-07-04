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
#include "bitxorcore/utils/ExceptionLogging.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace thread { class IoThreadPool; } }

namespace bitxorcore { namespace tools {

	/// Gets the value associated with \a future and forwards it to \a action on success.
	/// On failure, \c false is returned and \a description is included in a warning log.
	template<typename TAction, typename TFuture>
	bool UnwrapFutureAndSuppressErrors(const char* description, TFuture&& future, TAction action) {
		try {
			action(future.get());
			return true;
		} catch (...) {
			// suppress
			BITXORCORE_LOG(warning) << UNHANDLED_EXCEPTION_MESSAGE(description);
			return false;
		}
	}

	/// Creates a started thread pool with \a numThreads threads.
	std::unique_ptr<thread::IoThreadPool> CreateStartedThreadPool(uint32_t numThreads = 1);
}}
