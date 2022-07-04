cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.config)
target_link_libraries(bitxorcore.config bitxorcore.ionet)
