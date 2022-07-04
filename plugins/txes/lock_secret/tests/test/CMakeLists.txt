cmake_minimum_required(VERSION 3.14)

bitxorcore_plugin_test_library(tests.bitxorcore.test.plugins.locksecret)
target_link_libraries(tests.bitxorcore.test.plugins.locksecret bitxorcore.plugins.locksecret.deps)
