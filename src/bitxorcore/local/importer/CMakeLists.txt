cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.local.importer)
target_link_libraries(bitxorcore.local.importer bitxorcore.local.recovery bitxorcore.local.server)
