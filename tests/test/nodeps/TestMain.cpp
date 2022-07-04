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

#include "bitxorcore/utils/ConfigurationValueParsers.h"
#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/version/version.h"
#include "bitxorcore/preprocessor.h"
#include "tests/TestHarness.h"
#include <thread>

#ifdef BITXORCORE_DOCKER_TESTS
extern int global_argc;
extern char** global_argv;
int global_argc;
char** global_argv;
#endif

namespace bitxorcore { namespace test {

	namespace {
		uint32_t global_stress_iteration_count = 0;
	}

	uint32_t GetStressIterationCount() {
		return global_stress_iteration_count;
	}

	unsigned short GetLocalHostPort() {
		return GetStressIterationCount() ? 3014 : 2014;
	}

	uint32_t GetMaxNonDeterministicTestRetries() {
		return GetStressIterationCount() ? 500 : 25;
	}

	uint32_t GetNumDefaultPoolThreads() {
		return std::max<uint32_t>(16, 2 * std::thread::hardware_concurrency());
	}

	namespace {
		std::shared_ptr<void> SetupLogging() {
			utils::BasicLoggerOptions options;
			options.SinkType = utils::LogSinkType::Sync;
#ifndef _MSC_VER
			options.ColorMode = utils::LogColorMode::Ansi;
#endif

			auto pBootstrapper = std::make_shared<utils::LoggingBootstrapper>();
			pBootstrapper->addConsoleLogger(options, utils::LogFilter(utils::LogLevel::debug));
			return PORTABLE_MOVE(pBootstrapper);
		}

		uint32_t GetArgumentUint32(const std::string& name, int argc, char** argv) {
			auto key = "--cat_" + name + "=";
			for (auto i = 0; i < argc; ++i) {
				auto argumentKeyValue = std::string(argv[i]);
				if (0 != argumentKeyValue.find(key))
					continue;

				uint32_t parsedValue;
				auto value = argumentKeyValue.substr(key.size());
				if (!utils::TryParseValue(value, parsedValue))
					BITXORCORE_LOG(warning) << "argument '" << name << "' has invalid value: " << value;

				return parsedValue;
			}

			return 0;
		}
	}
}}

int main(int argc, char** argv) {
	bitxorcore::version::WriteVersionInformation(std::cout);
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	std::cout << "Initializing Logging..." << std::endl;
	auto pLoggingGuard = bitxorcore::test::SetupLogging();

	std::cout << "Initializing and Running Tests..." << std::endl;
	::testing::InitGoogleTest(&argc, argv);

#ifdef BITXORCORE_DOCKER_TESTS
	global_argc = argc;
	global_argv = argv;
#endif

	if (argc >= 2) {
		auto& count = bitxorcore::test::global_stress_iteration_count;
		count = bitxorcore::test::GetArgumentUint32("stress", argc, argv);
		BITXORCORE_LOG(warning) << "Note: BitxorCore Test stress iteration count = " << count << std::endl;
	}

	return RUN_ALL_TESTS();
}
