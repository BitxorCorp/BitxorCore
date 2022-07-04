cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.consumers)
target_link_libraries(bitxorcore.consumers bitxorcore.chain)
