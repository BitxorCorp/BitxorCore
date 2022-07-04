cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.mongo mocks)
bitxorcore_add_mongo_dependencies(tests.bitxorcore.test.mongo)
target_link_libraries(tests.bitxorcore.test.mongo tests.bitxorcore.test.core ${GTEST_LIBRARIES})
