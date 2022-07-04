cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.nodeps)
target_link_libraries(tests.bitxorcore.test.nodeps bitxorcore.utils bitxorcore.version ${GTEST_LIBRARIES} ${CMAKE_DL_LIBS})
