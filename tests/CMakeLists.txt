cmake_minimum_required(VERSION 3.14)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(bench)
add_subdirectory(bitxorcore)
add_subdirectory(test)
add_subdirectory(int)
