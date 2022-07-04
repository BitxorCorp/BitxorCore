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

#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/thread/ThreadGroup.h"
#include "bitxorcore/utils/StackLogger.h"
#include "tests/bitxorcore/utils/test/LoggingTestUtils.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace utils {

#define TEST_CLASS BitxorCoreLoggingTests

	namespace {
		void LogAllLevelsWithDefaultMacro() {
			BITXORCORE_LOG(trace) << "alice trace message";
			BITXORCORE_LOG(info) << "foo info";
			BITXORCORE_LOG(debug) << "bob debug message";
			BITXORCORE_LOG(warning) << "bar warning";
			BITXORCORE_LOG(important) << "charlie important message!";
			BITXORCORE_LOG(fatal) << "fatal termination";
			BITXORCORE_LOG(error) << "baz error";
		}

		void LogAllLevelsWithExplicitLevelMacro() {
			BITXORCORE_LOG_LEVEL(LogLevel::trace) << "alice trace message";
			BITXORCORE_LOG_LEVEL(LogLevel::info) << "foo info";
			BITXORCORE_LOG_LEVEL(LogLevel::debug) << "bob debug message";
			BITXORCORE_LOG_LEVEL(LogLevel::warning) << "bar warning";
			BITXORCORE_LOG_LEVEL(LogLevel::important) << "charlie important message!";
			BITXORCORE_LOG_LEVEL(LogLevel::fatal) << "fatal termination";
			BITXORCORE_LOG_LEVEL(LogLevel::error) << "baz error";
		}

		void LogMessagesWithSubcomponentTag(const char* tag) {
#define BITXORCORE_LOG_TAG(LEVEL) \
	BITXORCORE_LOG_WITH_LOGGER_LEVEL_TAG( \
		log::global_logger::get(), \
		LEVEL, \
		RawString(tag, strlen(tag)))

			BITXORCORE_LOG_TAG(LogLevel::trace) << "alice trace message";
			BITXORCORE_LOG_TAG(LogLevel::info) << "foo info";
			BITXORCORE_LOG_TAG(LogLevel::error) << "baz error";
#undef BITXORCORE_LOG_TAG
		}

		void AddUnfilteredFileLogger(LoggingBootstrapper& bootstrapper) {
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), LogFilter(LogLevel::min));
		}
	}

	TEST(TEST_CLASS, CanOutputLogLevel) {
		EXPECT_EQ("trace", test::ToString(utils::LogLevel::trace));
		EXPECT_EQ("debug", test::ToString(utils::LogLevel::debug));
		EXPECT_EQ("info", test::ToString(utils::LogLevel::info));
		EXPECT_EQ("important", test::ToString(utils::LogLevel::important));
		EXPECT_EQ("warning", test::ToString(utils::LogLevel::warning));
		EXPECT_EQ("error", test::ToString(utils::LogLevel::error));
		EXPECT_EQ("fatal", test::ToString(utils::LogLevel::fatal));

		EXPECT_EQ("trace", test::ToString(utils::LogLevel::min));
		EXPECT_EQ("fatal", test::ToString(utils::LogLevel::max));

		EXPECT_EQ("fatal", test::ToString(static_cast<utils::LogLevel>(0xFFFF)));
	}

	TEST(TEST_CLASS, CanWriteLogMessagesWithBitxorCoreLogMacro) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger
			LoggingBootstrapper bootstrapper;
			AddUnfilteredFileLogger(bootstrapper);

			// Act: log messages
			LogAllLevelsWithDefaultMacro();
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<trace> (utils::LoggingTests.cpp@36) alice trace message",
			"<info> (utils::LoggingTests.cpp@37) foo info",
			"<debug> (utils::LoggingTests.cpp@38) bob debug message",
			"<warning> (utils::LoggingTests.cpp@39) bar warning",
			"<important> (utils::LoggingTests.cpp@40) charlie important message!",
			"<fatal> (utils::LoggingTests.cpp@41) fatal termination",
			"<error> (utils::LoggingTests.cpp@42) baz error"
		});
	}

	TEST(TEST_CLASS, CanWriteLogMessagesWithBitxorCoreLogLevelMacro) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger
			LoggingBootstrapper bootstrapper;
			AddUnfilteredFileLogger(bootstrapper);

			// Act: log messages
			LogAllLevelsWithExplicitLevelMacro();
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<trace> (utils::LoggingTests.cpp@46) alice trace message",
			"<info> (utils::LoggingTests.cpp@47) foo info",
			"<debug> (utils::LoggingTests.cpp@48) bob debug message",
			"<warning> (utils::LoggingTests.cpp@49) bar warning",
			"<important> (utils::LoggingTests.cpp@50) charlie important message!",
			"<fatal> (utils::LoggingTests.cpp@51) fatal termination",
			"<error> (utils::LoggingTests.cpp@52) baz error"
		});
	}

	TEST(TEST_CLASS, CanWriteLogMessagesWithCustomComponentTags) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger
			LoggingBootstrapper bootstrapper;
			AddUnfilteredFileLogger(bootstrapper);

			// Act: log messages with custom tags
			LogMessagesWithSubcomponentTag("foo");
			LogMessagesWithSubcomponentTag("bar");
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<trace> (foo::LoggingTests.cpp@62) alice trace message",
			"<info> (foo::LoggingTests.cpp@63) foo info",
			"<error> (foo::LoggingTests.cpp@64) baz error",
			"<trace> (bar::LoggingTests.cpp@62) alice trace message",
			"<info> (bar::LoggingTests.cpp@63) foo info",
			"<error> (bar::LoggingTests.cpp@64) baz error"
		});
	}

	TEST(TEST_CLASS, CanFilterMessagesBySettingGlobalLevel) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger and filter out some messages
			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), LogFilter(LogLevel::info));

			// Act: log messages
			LogAllLevelsWithDefaultMacro();
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<info> (utils::LoggingTests.cpp@37) foo info",
			"<warning> (utils::LoggingTests.cpp@39) bar warning",
			"<important> (utils::LoggingTests.cpp@40) charlie important message!",
			"<fatal> (utils::LoggingTests.cpp@41) fatal termination",
			"<error> (utils::LoggingTests.cpp@42) baz error"
		});
	}

	TEST(TEST_CLASS, CanFilterMessagesBySettingComponentFilterLevelAboveGlobalLevel) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger and filter out some messages
			LogFilter filter(LogLevel::info);
			filter.setLevel("foo", LogLevel::error); // foo messages below error should not appear

			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), filter);

			// Act: log messages with custom tags
			LogMessagesWithSubcomponentTag("foo");
			LogMessagesWithSubcomponentTag("bar");
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<error> (foo::LoggingTests.cpp@64) baz error",
			"<info> (bar::LoggingTests.cpp@63) foo info",
			"<error> (bar::LoggingTests.cpp@64) baz error"
		});
	}

	TEST(TEST_CLASS, CanFilterMessagesBySettingComponentFilterLevelBelowGlobalLevel) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger and filter out some messages
			LogFilter filter(LogLevel::info);
			filter.setLevel("foo", LogLevel::min); // all foo messages should appear

			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), filter);

			// Act: log messages with custom tags
			LogMessagesWithSubcomponentTag("foo");
			LogMessagesWithSubcomponentTag("bar");
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<trace> (foo::LoggingTests.cpp@62) alice trace message",
			"<info> (foo::LoggingTests.cpp@63) foo info",
			"<error> (foo::LoggingTests.cpp@64) baz error",
			"<info> (bar::LoggingTests.cpp@63) foo info",
			"<error> (bar::LoggingTests.cpp@64) baz error"
		});
	}

	TEST(TEST_CLASS, CanFilterMessagesFromRealComponents) {
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: add a file logger and filter out some messages
			LogFilter filter(LogLevel::trace);
			filter.setLevel("utils", LogLevel::max); // filter out all non-fatal utils messages

			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), filter);

			// Act: use a stack logger to generate some logs
			StackLogger stackLogger("test", LogLevel::info);

			// - log messages with custom tags
			LogMessagesWithSubcomponentTag("bar");
		}

		// Assert:
		auto records = test::ParseLogLines(logFileGuard.name());
		test::AssertTimestampsAreIncreasing(records);
		test::AssertNumUniqueThreadIds(records, 1);
		test::AssertMessages(records, {
			"<trace> (bar::LoggingTests.cpp@62) alice trace message",
			"<info> (bar::LoggingTests.cpp@63) foo info",
			"<error> (bar::LoggingTests.cpp@64) baz error"
		});
	}

	TEST(TEST_CLASS, CanLogAndFilterMessagesFromMultipleThreads) {
		// Arrange:
		test::TempLogsDirectoryGuard logFileGuard;

		{
			// Arrange: construct the tag for each thread
			std::vector<std::string> idStrings;
			for (auto i = 0u; i < test::GetNumDefaultPoolThreads(); ++i)
				idStrings.push_back(std::to_string(i + 1));

			// - add a file logger and filter out some messages
			LogFilter filter(LogLevel::info);
			filter.setLevel("io", LogLevel::max);

			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), filter);

			// Act: create a thread pool and write logs from each thread
			std::atomic<size_t> numWaitingThreads(0);
			thread::ThreadGroup threads;
			for (auto i = 0u; i < test::GetNumDefaultPoolThreads(); ++i) {
				threads.spawn([&numWaitingThreads, &idStrings] {
					auto id = ++numWaitingThreads;
					while (test::GetNumDefaultPoolThreads() != numWaitingThreads) {}

					LogMessagesWithSubcomponentTag(idStrings[id - 1].c_str());
				});
			}

			threads.join();
		}

		// Assert:
		auto records = test::ParseMultiThreadedLogLines(logFileGuard.name());
		EXPECT_EQ(test::GetNumDefaultPoolThreads(), records.size());

		// - all threads should output the same messages with unique subcomponent tags
		std::set<std::string> subcomponents;
		for (const auto& pair : records) {
			const auto& subcomponent = pair.second.back().Subcomponent;
			test::AssertTimestampsAreIncreasing(pair.second);
			AssertMessages(pair.second, {
				"<info> (" + subcomponent + "::LoggingTests.cpp@63) foo info",
				"<error> (" + subcomponent + "::LoggingTests.cpp@64) baz error"
			});
			subcomponents.insert(subcomponent);
		}

		// - all subcomponents (unique per thread in this test) should be represented
		std::set<std::string> expectedSubcomponents;
		for (auto i = 0u; i < test::GetNumDefaultPoolThreads(); ++i)
			expectedSubcomponents.insert(std::to_string(i + 1));

		EXPECT_EQ(expectedSubcomponents, subcomponents);
	}

	TEST(TEST_CLASS, CanConfigureLoggersWithIndependentFilters) {
		test::TempLogsDirectoryGuard logFileGuard;
		test::TempLogsDirectoryGuard logSecondaryFileGuard("BitxorCoreLoggingTests_Secondary");

		{
			// Arrange: add two file loggers with different filters
			LogFilter secondaryLogFilter(LogLevel::warning);
			secondaryLogFilter.setLevel("foo", LogLevel::min); // all foo messages should appear
			secondaryLogFilter.setLevel("bar", LogLevel::max); // no non-fatal bar messages should appear

			LoggingBootstrapper bootstrapper;
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions(), LogFilter(LogLevel::info));
			bootstrapper.addFileLogger(test::CreateTestFileLoggerOptions("BitxorCoreLoggingTests_Secondary"), secondaryLogFilter);

			// Act: log messages with custom tags
			LogMessagesWithSubcomponentTag("foo");
			LogMessagesWithSubcomponentTag("bar");
		}

		// Assert: primary log is composed of all messages at least informational level
		{
			BITXORCORE_LOG(debug) << "checking primary log";
			auto primaryRecords = test::ParseLogLines(logFileGuard.name());
			test::AssertTimestampsAreIncreasing(primaryRecords);
			test::AssertNumUniqueThreadIds(primaryRecords, 1);
			test::AssertMessages(primaryRecords, {
				"<info> (foo::LoggingTests.cpp@63) foo info",
				"<error> (foo::LoggingTests.cpp@64) baz error",
				"<info> (bar::LoggingTests.cpp@63) foo info",
				"<error> (bar::LoggingTests.cpp@64) baz error"
			});
		}

		// - secondary log is composed of foo messages but not bar messages
		{
			BITXORCORE_LOG(debug) << "checking secondary log";
			auto secondaryRecords = test::ParseLogLines(logSecondaryFileGuard.name());
			test::AssertTimestampsAreIncreasing(secondaryRecords);
			test::AssertNumUniqueThreadIds(secondaryRecords, 1);
			test::AssertMessages(secondaryRecords, {
				"<trace> (foo::LoggingTests.cpp@62) alice trace message",
				"<info> (foo::LoggingTests.cpp@63) foo info",
				"<error> (foo::LoggingTests.cpp@64) baz error"
			});
		}
	}
}}
