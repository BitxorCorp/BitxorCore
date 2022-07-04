cmake_minimum_required(VERSION 3.14)

add_subdirectory(plugins)

bitxorcore_library_target(bitxorcore.tools)
target_link_libraries(bitxorcore.tools ${CORE_BITXORCORE_LIBS} bitxorcore.api bitxorcore.config bitxorcore.net bitxorcore.sdk bitxorcore.version)
