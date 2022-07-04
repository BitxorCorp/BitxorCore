cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.validators)
target_link_libraries(bitxorcore.validators bitxorcore.cache bitxorcore.thread)
