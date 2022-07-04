cmake_minimum_required(VERSION 3.14)

# setup benchmark
message("--- locating bench dependencies ---")
find_package(benchmark 1.5.3 EXACT REQUIRED)
message("bench     ver: ${benchmark_VERSION}")

# add benchmark dependencies
function(bitxorcore_add_benchmark_dependencies TARGET_NAME)
	# this should both link and add proper include path
	target_link_libraries(${TARGET_NAME} benchmark::benchmark)
endfunction()

# used to define a bitxorcore bench executable
function(bitxorcore_bench_executable_target TARGET_NAME)
	bitxorcore_executable(${TARGET_NAME} ${ARGN})

	bitxorcore_add_benchmark_dependencies(${TARGET_NAME})
	bitxorcore_set_test_compiler_options()

	bitxorcore_target(${TARGET_NAME})

	add_dependencies(tools ${TARGET_NAME})
	install(TARGETS ${TARGET_NAME})
endfunction()

add_subdirectory(crypto)

add_subdirectory(nodeps)
