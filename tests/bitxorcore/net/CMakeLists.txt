cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.net net)
target_link_libraries(tests.bitxorcore.net bitxorcore.api)
