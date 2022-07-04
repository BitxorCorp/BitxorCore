cmake_minimum_required(VERSION 3.14)

bitxorcore_bench_executable_target(bench.bitxorcore.crypto.verify)
target_link_libraries(bench.bitxorcore.crypto.verify bitxorcore.crypto bench.bitxorcore.bench.nodeps)
