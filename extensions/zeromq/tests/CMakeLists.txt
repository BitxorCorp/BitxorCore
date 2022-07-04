cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.zeromq)

bitxorcore_test_executable_target(${TARGET_NAME} core test)
bitxorcore_add_zeromq_dependencies(${TARGET_NAME})

set_property(TEST ${TARGET_NAME} PROPERTY LABELS Integration)
