cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.subscribers cache test)
target_link_libraries(tests.bitxorcore.subscribers bitxorcore.config)
