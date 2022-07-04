cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.cache)
target_link_libraries(tests.bitxorcore.test.cache tests.bitxorcore.test.core bitxorcore.cache bitxorcore.cache_core bitxorcore.cache_tx)
