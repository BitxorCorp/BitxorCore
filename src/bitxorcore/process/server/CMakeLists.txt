cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.server)

bitxorcore_executable(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} bitxorcore.local.server bitxorcore.process)
bitxorcore_target(${TARGET_NAME})

add_dependencies(server ${TARGET_NAME})
