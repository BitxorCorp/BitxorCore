cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.cache_db)

bitxorcore_library_target(${TARGET_NAME})
bitxorcore_add_rocksdb_dependencies(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} bitxorcore.config bitxorcore.tree)
