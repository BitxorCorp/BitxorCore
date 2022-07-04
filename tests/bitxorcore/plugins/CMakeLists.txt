cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.plugins core)
target_link_libraries(tests.bitxorcore.plugins bitxorcore.plugins.transfer) # allow transfer to be loaded implicitly
