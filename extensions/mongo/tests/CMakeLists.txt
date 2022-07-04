cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.mongo)

add_subdirectory(int)
add_subdirectory(test)

bitxorcore_test_executable_target(${TARGET_NAME} mongo mappers)
bitxorcore_add_mongo_dependencies(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} tests.bitxorcore.test.cache)
target_link_libraries(${TARGET_NAME} bitxorcore.mongo.plugins.transfer) # allow transfer to be loaded implicitly
