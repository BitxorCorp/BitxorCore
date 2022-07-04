cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.local.recovery local)
target_link_libraries(tests.bitxorcore.local.recovery tests.bitxorcore.test.genesis)
