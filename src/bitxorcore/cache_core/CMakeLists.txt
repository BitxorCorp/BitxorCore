cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.cache_core)
target_link_libraries(bitxorcore.cache_core bitxorcore.cache bitxorcore.state)
