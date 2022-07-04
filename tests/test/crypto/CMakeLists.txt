cmake_minimum_required(VERSION 3.14)

include_directories(../../../external)

bitxorcore_library_target(tests.bitxorcore.test.crypto)
target_link_libraries(tests.bitxorcore.test.crypto tests.bitxorcore.test.nodeps)
bitxorcore_add_openssl_dependencies(tests.bitxorcore.test.crypto)
