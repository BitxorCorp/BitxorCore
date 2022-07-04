cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.subscribers)
target_link_libraries(bitxorcore.subscribers bitxorcore.cache bitxorcore.cache_tx bitxorcore.config bitxorcore.validators)
