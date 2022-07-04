cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.extensions local)
target_link_libraries(tests.bitxorcore.extensions bitxorcore.plugins.coresystem.deps tests.bitxorcore.test.genesis)
bitxorcore_add_openssl_dependencies(tests.bitxorcore.extensions)
