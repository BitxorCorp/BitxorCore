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

#include "ProcessMain.h"
#include "Platform.h"
#include "Signals.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/config/ValidateConfiguration.h"
#include "bitxorcore/crypto/OpensslMemory.h"
#include "bitxorcore/io/FileLock.h"
#include "bitxorcore/thread/ThreadInfo.h"
#include "bitxorcore/utils/ExceptionLogging.h"
#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/version/version.h"
#include "bitxorcore/preprocessor.h"
#include <iostream>

namespace bitxorcore { namespace process {

	namespace {
		// region initialization utils

		config::BitxorCoreConfiguration LoadConfiguration(int argc, const char** argv, const std::string& extensionsHost) {
			auto resourcesPath = GetResourcesPath(argc, argv);
			std::cout << "loading resources from " << resourcesPath << std::endl;
			return config::BitxorCoreConfiguration::LoadFromPath(resourcesPath, extensionsHost);
		}

		std::unique_ptr<utils::LogFilter> CreateLogFilter(const config::BasicLoggerConfiguration& config) {
			auto pFilter = std::make_unique<utils::LogFilter>(config.Level);
			for (const auto& pair : config.ComponentLevels)
				pFilter->setLevel(pair.first.c_str(), pair.second);

			return pFilter;
		}

		std::shared_ptr<void> SetupLogging(const std::string& host, const config::LoggingConfiguration& config) {
			auto pBootstrapper = std::make_shared<utils::LoggingBootstrapper>();

			pBootstrapper->addFileLogger(config::GetFileLoggerOptions(config.File), *CreateLogFilter(config.File));

			// log version only to file before initializing console
			std::ostringstream versionStream;
			versionStream << "bitxorcore " << host << std::endl;
			version::WriteVersionInformation(versionStream);
			BITXORCORE_LOG(important) << versionStream.str();

			pBootstrapper->addConsoleLogger(config::GetConsoleLoggerOptions(config.Console), *CreateLogFilter(config.Console));
			return PORTABLE_MOVE(pBootstrapper);
		}

		[[noreturn]]
		void TerminateHandler() noexcept {
			// 1. if termination is caused by an exception, log it
			if (std::current_exception()) {
				BITXORCORE_LOG(fatal)
						<< std::endl << "thread: " << thread::GetThreadName()
						<< std::endl << UNHANDLED_EXCEPTION_MESSAGE("running local node");
			}

			// 2. flush the log and abort
			utils::BitxorCoreLogFlush();
			std::abort();
		}

		// endregion

		void Run(config::BitxorCoreConfiguration&& config, ProcessOptions processOptions, const CreateProcessHost& createProcessHost) {
			auto bitxorcoreKeys = config::BitxorCoreKeys(config.User.CertificateDirectory);

			BITXORCORE_LOG(important)
					<< "booting process with keys:"
					<< std::endl << " -   CA " << bitxorcoreKeys.caPublicKey()
					<< std::endl << " - NODE " << bitxorcoreKeys.nodeKeyPair().publicKey();
			auto pProcessHost = createProcessHost(std::move(config), bitxorcoreKeys);

			if (ProcessOptions::Exit_After_Termination_Signal == processOptions)
				WaitForTerminationSignal();

			BITXORCORE_LOG(important) << "SHUTTING DOWN PROCESS";
			pProcessHost.reset();
		}
	}

	std::filesystem::path GetResourcesPath(int argc, const char** argv) {
		return std::filesystem::path(argc > 1 ? argv[1] : "..") / "resources";
	}

	int ProcessMain(int argc, const char** argv, const std::string& host, const CreateProcessHost& createProcessHost) {
		return ProcessMain(argc, argv, host, ProcessOptions::Exit_After_Termination_Signal, createProcessHost);
	}

	int ProcessMain(
			int argc,
			const char** argv,
			const std::string& host,
			ProcessOptions processOptions,
			const CreateProcessHost& createProcessHost) {
		std::set_terminate(&TerminateHandler);
		thread::SetThreadName(host + " bitxorcore");
		version::WriteVersionInformation(std::cout);

		crypto::SetupOpensslMemoryFunctions();

		// 1. load and validate the configuration
		auto config = LoadConfiguration(argc, argv, host);
		ValidateConfiguration(config);

		// 2. initialize logging
		auto pLoggingGuard = SetupLogging(host, config.Logging);

		// 3. check instance
		std::filesystem::path lockFilePath = config.User.DataDirectory;
		lockFilePath /= host + ".lock";
		io::FileLock instanceLock(lockFilePath.generic_string());
		if (!instanceLock.try_lock()) {
			BITXORCORE_LOG(fatal) << "could not acquire instance lock " << lockFilePath;
			return -3;
		}

		// 4. platform specific settings
		PlatformSettings();

		// 5. run the server
		Run(std::move(config), processOptions, createProcessHost);
		return 0;
	}
}}
