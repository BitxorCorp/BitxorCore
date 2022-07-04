cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.local.server)
target_link_libraries(bitxorcore.local.server bitxorcore.local bitxorcore.handlers bitxorcore.net)
