cmake_minimum_required(VERSION 3.14)

set(PLUGIN_DEPS_FOLDERS model observers validators)

include_directories(.)
add_subdirectory(src)
bitxorcore_add_tests_subdirectory(tests)
