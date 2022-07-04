cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.process)
target_link_libraries(bitxorcore.process bitxorcore.version)

add_subdirectory(broker)
add_subdirectory(importer)
add_subdirectory(recovery)
add_subdirectory(server)

install(TARGETS bitxorcore.broker bitxorcore.importer bitxorcore.recovery bitxorcore.server)
