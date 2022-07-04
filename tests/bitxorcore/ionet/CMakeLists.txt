cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.ionet net)
bitxorcore_add_openssl_dependencies(tests.bitxorcore.ionet)
