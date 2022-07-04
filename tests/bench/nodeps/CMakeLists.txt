cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bench.bitxorcore.bench.nodeps)
bitxorcore_add_benchmark_dependencies(bench.bitxorcore.bench.nodeps)
target_link_libraries(bench.bitxorcore.bench.nodeps bitxorcore.utils bitxorcore.version)
