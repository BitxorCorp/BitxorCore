cmake_minimum_required(VERSION 3.14)

bitxorcore_mongo_plugin_test_library(tests.bitxorcore.test.mongo.plugins.lockhash)
target_link_libraries(tests.bitxorcore.test.mongo.plugins.lockhash bitxorcore.plugins.lockhash.deps bitxorcore.mongo.plugins.lockhash)
