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
#include <sstream>
#include <gtest/gtest.h>

namespace bitxorcore { namespace test {

// redefine test macro to allow running tests with stress configuration
#undef TEST

#define BITXORCORE_TEST_FRIENDLY_NAME_(TEST_FIXTURE, TEST_NAME) #TEST_FIXTURE "::" #TEST_NAME
#define BITXORCORE_TEST_IMPL_NAME_(TEST_FIXTURE, TEST_NAME) TEST_FIXTURE##_##TEST_NAME##_Impl

#define TEST(TEST_FIXTURE, TEST_NAME) \
	void BITXORCORE_TEST_IMPL_NAME_(TEST_FIXTURE, TEST_NAME)(); \
	\
	GTEST_TEST(TEST_FIXTURE, TEST_NAME) { \
		/* if stress is disabled, just call test function */ \
		if (!test::GetStressIterationCount()) { \
			BITXORCORE_TEST_IMPL_NAME_(TEST_FIXTURE, TEST_NAME)(); \
			return; \
		} \
		\
		/* if stress is enabled, call in loop with iteration counter */ \
		for (auto stressCounter = 1u; stressCounter <= test::GetStressIterationCount(); ++stressCounter) { \
			const auto Test_Fqn = BITXORCORE_TEST_FRIENDLY_NAME_(TEST_FIXTURE, TEST_NAME); \
			BITXORCORE_LOG(debug) << "---- iter " << stressCounter << "/" << test::GetStressIterationCount() << " " << Test_Fqn << " ----"; \
			\
			try { \
				BITXORCORE_TEST_IMPL_NAME_(TEST_FIXTURE, TEST_NAME)(); \
			} catch (...) { \
				/* flatten error information into std::runtime_error for better jenkins reporting */ \
				std::ostringstream exceptionMessage; \
				exceptionMessage \
						<< "unhandled exception during " \
						<< Test_Fqn << " iteration " << stressCounter << "!" \
						<< EXCEPTION_DIAGNOSTIC_MESSAGE(); \
				BITXORCORE_LOG(fatal) << exceptionMessage.str(); \
				throw std::runtime_error(exceptionMessage.str().c_str()); \
			} \
			\
			if (!::testing::Test::HasFailure()) \
				continue; \
			\
			BITXORCORE_LOG(error) << Test_Fqn << " failed on iteration " << stressCounter; \
			return; \
		} \
	} \
	\
	void BITXORCORE_TEST_IMPL_NAME_(TEST_FIXTURE, TEST_NAME)()

// NO_STRESS_TEST should be used by tests that shouldn't be stressed
#define NO_STRESS_TEST(TEST_FIXTURE, TEST_NAME) GTEST_TEST(TEST_FIXTURE, TEST_NAME)
}}
