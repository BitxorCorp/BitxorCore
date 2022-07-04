cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.thread)
target_link_libraries(bitxorcore.thread bitxorcore.model bitxorcore.utils)
