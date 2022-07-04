cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.api)
target_link_libraries(bitxorcore.api bitxorcore.io bitxorcore.ionet bitxorcore.model)
