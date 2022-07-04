cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.cache cache test)
target_link_libraries(tests.bitxorcore.cache bitxorcore.crypto)
bitxorcore_add_rocksdb_dependencies(tests.bitxorcore.cache)
