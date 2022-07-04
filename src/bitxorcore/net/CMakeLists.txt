cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.net)
target_link_libraries(bitxorcore.net bitxorcore.ionet bitxorcore.thread)
