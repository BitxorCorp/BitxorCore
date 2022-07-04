cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.net mocks)
target_link_libraries(tests.bitxorcore.test.net tests.bitxorcore.test.core tests.bitxorcore.test.crypto bitxorcore.net)
bitxorcore_add_openssl_dependencies(tests.bitxorcore.test.net)
