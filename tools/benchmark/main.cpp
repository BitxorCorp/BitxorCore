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

#include "tools/ToolMain.h"
#include "tools/ToolKeys.h"
#include "tools/ToolThreadUtils.h"
#include "bitxorcore/crypto/Signer.h"
#include "bitxorcore/thread/IoThreadPool.h"
#include "bitxorcore/thread/ParallelFor.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace tools { namespace benchmark {

	namespace {
		struct BenchmarkEntry {
			std::vector<uint8_t> Data;
			bitxorcore::Signature Signature;
			bool IsVerified = false;
		};

		class BenchmarkTool : public Tool {
		public:
			std::string name() const override {
				return "Benchmark Tool";
			}

			void prepareOptions(OptionsBuilder& optionsBuilder, OptionsPositional&) override {
				optionsBuilder("num threads,t",
						OptionsValue<uint32_t>(m_numThreads)->default_value(0),
						"number of threads");
				optionsBuilder("num partitions,p",
						OptionsValue<uint32_t>(m_numPartitions)->default_value(0),
						"number of partitions");
				optionsBuilder("ops / partition,o",
						OptionsValue<uint32_t>(m_opsPerPartition)->default_value(1000),
						"number of operations per partition");
				optionsBuilder("data size,s",
						OptionsValue<uint32_t>(m_dataSize)->default_value(148),
						"size of the data to generate");
			}

			int run(const Options&) override {
				m_numThreads = 0 != m_numThreads ? m_numThreads : std::thread::hardware_concurrency();
				m_numPartitions = 0 != m_numPartitions ? m_numPartitions : m_numThreads;

				BITXORCORE_LOG(info)
						<< "num threads (" << m_numThreads
						<< "), num partitions (" << m_numPartitions
						<< "), ops / partition (" << m_opsPerPartition
						<< "), data size (" << m_dataSize << ")";

				auto keyPair = GenerateRandomKeyPair();
				auto entries = std::vector<BenchmarkEntry>(m_numPartitions * m_opsPerPartition);
				auto pPool = CreateStartedThreadPool(m_numThreads);

				BITXORCORE_LOG(info) << "num operations (" << entries.size() << ")";

				RunParallel("Data Generation", *pPool, entries, [dataSize = m_dataSize](auto& entry) {
					entry.Data.resize(dataSize);
					std::generate_n(entry.Data.begin(), entry.Data.size(), []() { return static_cast<uint8_t>(std::rand()); });
				});

				RunParallel("Signature", *pPool, entries, [&keyPair](auto& entry) {
					crypto::Sign(keyPair, entry.Data, entry.Signature);
				});

				RunParallel("Verify", *pPool, entries, [&keyPair](auto& entry) {
					entry.IsVerified = crypto::Verify(keyPair.publicKey(), entry.Data, entry.Signature);
					if (!entry.IsVerified)
						BITXORCORE_LOG(warning) << "could not verify data!";
				});

				return 0;
			}

		private:
			template<typename TAction>
			uint64_t RunParallel(
					const char* testName,
					thread::IoThreadPool& pool,
					std::vector<BenchmarkEntry>& entries,
					TAction action) const {
				utils::StackLogger logger(testName, utils::LogLevel::info);
				utils::StackTimer stopwatch;
				thread::ParallelFor(pool.ioContext(), entries, m_numPartitions, [action](auto& entry, auto) {
					action(entry);
					return true;
				}).get();

				auto elapsedMillis = stopwatch.millis();
				auto elapsedMicrosPerOp = elapsedMillis * 1000u / entries.size();
				auto opsPerSecond = 0 == elapsedMillis ? 0 : entries.size() * 1000u / elapsedMillis;
				BITXORCORE_LOG(info)
						<< (0 == opsPerSecond ? "???" : std::to_string(opsPerSecond)) << " ops/s "
						<< "(elapsed time " << elapsedMillis << "ms, " << elapsedMicrosPerOp << "us/op)";
				return elapsedMillis;
			}

		private:
			uint32_t m_numThreads;
			uint32_t m_numPartitions;
			uint32_t m_opsPerPartition;
			uint32_t m_dataSize;
		};
	}
}}}

int main(int argc, const char** argv) {
	bitxorcore::tools::benchmark::BenchmarkTool benchmarkTool;
	return bitxorcore::tools::ToolMain(argc, argv, benchmarkTool);
}
