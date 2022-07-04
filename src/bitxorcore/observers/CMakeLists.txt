cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.observers)
target_link_libraries(bitxorcore.observers bitxorcore.cache)
