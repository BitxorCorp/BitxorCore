cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.cache_db)

bitxorcore_test_executable_target(${TARGET_NAME} nodeps test)
target_link_libraries(${TARGET_NAME} bitxorcore.io)
bitxorcore_add_rocksdb_dependencies(${TARGET_NAME})

set_property(TEST ${TARGET_NAME} PROPERTY LABELS Database_Rocks)
