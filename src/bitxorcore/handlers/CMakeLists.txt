cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.handlers)
target_link_libraries(bitxorcore.handlers bitxorcore.io bitxorcore.ionet bitxorcore.model)
