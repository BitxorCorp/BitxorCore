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

#include "bitxorcore/crypto/Hashes.h"
#include "tests/bench/nodeps/Random.h"
#include <benchmark/benchmark.h>

namespace bitxorcore { namespace crypto {

	namespace {
		// region traits

		struct Ripemd160_Traits {
			using HashType = Hash160;
			static constexpr auto HashFunc = Ripemd160;
		};

		struct Bitcoin160_Traits {
			using HashType = Hash160;
			static constexpr auto HashFunc = Bitcoin160;
		};

		struct Sha256Double_Traits {
			using HashType = Hash256;
			static constexpr auto HashFunc = Sha256Double;
		};

		struct Sha512_Traits {
			using HashType = Hash512;
			static constexpr auto HashFunc = Sha512;
		};

		struct Sha3_256_Traits {
			using HashType = Hash256;
			static constexpr auto HashFunc = Sha3_256;
		};

		// endregion

		template<typename TTraits>
		void BenchmarkHasher(benchmark::State& state) {
			std::vector<uint8_t> buffer(static_cast<size_t>(state.range(0)));
			typename TTraits::HashType hash;
			for (auto _ : state) {
				state.PauseTiming();
				bench::FillWithRandomData(buffer);
				state.ResumeTiming();

				TTraits::HashFunc(buffer, hash);
			}

			state.SetBytesProcessed(static_cast<int64_t>(buffer.size() * state.iterations()));
		}

		void AddDefaultArguments(benchmark::internal::Benchmark& benchmark) {
			for (auto arg : { 256, 1024, 4096, 16384})
				benchmark.UseRealTime()->Arg(arg);
		}
	}
}}

#define REGISTER_BENCHMARK(BENCH_NAME) benchmark::RegisterBenchmark(#BENCH_NAME, BENCH_NAME)

#define BITXORCORE_REGISTER_HASHER_BENCHMARK(TRAITS_NAME) \
	bitxorcore::crypto::AddDefaultArguments(*REGISTER_BENCHMARK(bitxorcore::crypto::BenchmarkHasher<bitxorcore::crypto::TRAITS_NAME>))

void RegisterTests();
void RegisterTests() {
	BITXORCORE_REGISTER_HASHER_BENCHMARK(Ripemd160_Traits);
	BITXORCORE_REGISTER_HASHER_BENCHMARK(Bitcoin160_Traits);
	BITXORCORE_REGISTER_HASHER_BENCHMARK(Sha256Double_Traits);
	BITXORCORE_REGISTER_HASHER_BENCHMARK(Sha512_Traits);
	BITXORCORE_REGISTER_HASHER_BENCHMARK(Sha3_256_Traits);
}
