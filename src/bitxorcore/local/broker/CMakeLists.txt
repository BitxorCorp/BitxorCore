cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.local.broker)
target_link_libraries(bitxorcore.local.broker bitxorcore.local)
