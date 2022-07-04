cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.plugins)
target_link_libraries(bitxorcore.plugins bitxorcore.validators ${CMAKE_DL_LIBS})
