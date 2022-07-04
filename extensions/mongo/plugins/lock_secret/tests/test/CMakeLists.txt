cmake_minimum_required(VERSION 3.14)

bitxorcore_mongo_plugin_test_library(tests.bitxorcore.test.mongo.plugins.locksecret)
target_link_libraries(tests.bitxorcore.test.mongo.plugins.locksecret bitxorcore.plugins.locksecret.deps bitxorcore.mongo.plugins.locksecret)
