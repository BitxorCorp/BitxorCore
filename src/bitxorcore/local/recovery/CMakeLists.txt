cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.local.recovery)
target_link_libraries(bitxorcore.local.recovery bitxorcore.local)
