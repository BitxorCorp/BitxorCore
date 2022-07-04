cmake_minimum_required(VERSION 3.14)

include_directories(../../../external)

bitxorcore_test_executable_target(tests.bitxorcore.crypto crypto)
target_link_libraries(tests.bitxorcore.crypto external)
bitxorcore_add_openssl_dependencies(tests.bitxorcore.crypto)
