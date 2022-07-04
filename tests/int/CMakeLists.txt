cmake_minimum_required(VERSION 3.14)

# used to define a bitxorcore int test executable by combining bitxorcore_test_executable and bitxorcore_target
function(bitxorcore_int_test_executable_target TARGET_NAME)
	bitxorcore_test_executable(${TARGET_NAME} ${ARGN})
	bitxorcore_target(${TARGET_NAME})

	set_property(TEST ${TARGET_NAME} PROPERTY LABELS Integration)

	# customize and export compiler options for gtest
	bitxorcore_set_test_compiler_options()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()

add_subdirectory(node)
add_subdirectory(stress)
