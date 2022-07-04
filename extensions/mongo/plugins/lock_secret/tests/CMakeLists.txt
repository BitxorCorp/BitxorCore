cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.mongo.plugins.locksecret)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/lock_secret)
bitxorcore_mongo_plugin_tests_with_deps(${TARGET_NAME} mappers)

add_subdirectory(int)
add_subdirectory(test)
