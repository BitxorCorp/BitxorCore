cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.zeromq)

bitxorcore_library_target(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} bitxorcore.model bitxorcore.thread)
bitxorcore_add_zeromq_dependencies(${TARGET_NAME})
