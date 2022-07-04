cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.local.broker local test)
target_link_libraries(tests.bitxorcore.local.broker bitxorcore.plugins.signature) # allow signature to be loaded implicitly
