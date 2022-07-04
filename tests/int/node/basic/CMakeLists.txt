cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.int.node)

bitxorcore_int_test_executable_target(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} tests.bitxorcore.int.node.test tests.bitxorcore.test.genesis bitxorcore.local.server)
