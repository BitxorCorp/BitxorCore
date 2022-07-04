cmake_minimum_required(VERSION 3.14)

set(PLUGIN_SDK_FOLDERS model)
set(PLUGIN_DEPS_FOLDERS config validators)

include_directories(.)
add_subdirectory(src)
bitxorcore_add_tests_subdirectory(tests)
