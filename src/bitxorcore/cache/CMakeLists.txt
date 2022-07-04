cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.cache)
target_link_libraries(bitxorcore.cache bitxorcore.cache_db bitxorcore.io bitxorcore.model bitxorcore.tree)
