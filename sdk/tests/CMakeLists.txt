cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.sdk)

bitxorcore_test_executable_target(${TARGET_NAME} core builders extensions parsers)
