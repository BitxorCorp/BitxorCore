cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.tools.bxorgen.blockhashes)

bitxorcore_library_target(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} bitxorcore.chain)

# tool has chain dependency so it must be able to access src
include_directories(${PROJECT_SOURCE_DIR}/src)
