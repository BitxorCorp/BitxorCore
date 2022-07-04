cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.io)
target_link_libraries(bitxorcore.io bitxorcore.config bitxorcore.model)
