cmake_minimum_required(VERSION 3.14)

bitxorcore_bench_executable_target(bench.bitxorcore.crypto.hashers)
target_link_libraries(bench.bitxorcore.crypto.hashers bitxorcore.crypto bench.bitxorcore.bench.nodeps)
