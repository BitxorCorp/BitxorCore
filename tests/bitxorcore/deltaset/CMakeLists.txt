cmake_minimum_required(VERSION 3.14)

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
endif()

bitxorcore_test_executable_target_header_only(tests.bitxorcore.deltaset nodeps test)
