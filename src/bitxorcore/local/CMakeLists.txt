cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.local)
target_link_libraries(bitxorcore.local bitxorcore.extensions)

add_subdirectory(broker)
add_subdirectory(importer)
add_subdirectory(recovery)
add_subdirectory(server)
