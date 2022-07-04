cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.tree)
target_link_libraries(bitxorcore.tree bitxorcore.crypto)
