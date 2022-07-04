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

#include "bitxorcore/utils/ReentrancyCheckReaderNotificationPolicy.h"
#include "bitxorcore/thread/ThreadGroup.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace utils {

#define TEST_CLASS ReentrancyCheckReaderNotificationPolicyTests

	TEST(TEST_CLASS, CannotAcquireReaderMultipleTimesFromSameThreadWithoutRelease) {
		// Arrange:
		ReentrancyCheckReaderNotificationPolicy policy;
		policy.readerAcquired();

		// Act + Assert:
		EXPECT_THROW(policy.readerAcquired(), reader_reentrancy_error);
	}

	TEST(TEST_CLASS, CanAcquireReaderFromSameThreadAfterRelease) {
		// Arrange:
		ReentrancyCheckReaderNotificationPolicy policy;
		policy.readerAcquired();

		// Act + Assert: no exception
		policy.readerReleased();
		policy.readerAcquired();
	}

	TEST(TEST_CLASS, CanAcquireReaderMultipleTimesFromMultipleThreads) {
		// Arrange:
		constexpr auto Num_Threads = 10u;
		ReentrancyCheckReaderNotificationPolicy policy;
		thread::ThreadGroup threads;

		// Act:
		std::atomic<size_t> counter(0);
		for (auto i = 0u; i < Num_Threads; ++i) {
			threads.spawn([&policy, &counter] {
				policy.readerAcquired();
				++counter;
			});
		}

		threads.join();

		// Assert:
		EXPECT_EQ(Num_Threads, counter);
	}
}}
