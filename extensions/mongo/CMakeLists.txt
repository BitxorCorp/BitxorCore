cmake_minimum_required(VERSION 3.14)

message("--- locating mongo dependencies ---")
find_package(MONGOCXX 3.6.3 EXACT REQUIRED)
find_package(MONGOC-1.0 1.17.5 EXACT REQUIRED)

message("mongocxx  ver: ${MONGOCXX_VERSION}")
message("mongoc    ver: ${MONGOC-1.0_VERSION}")
get_library_path("MONGOCXX_LIBRARY_PATH" mongo::mongocxx_shared)
message("mongocxx  lib: ${MONGOCXX_LIBRARY_PATH}")

### find and set mongo includes and libraries
function(bitxorcore_add_mongo_dependencies TARGET_NAME)
	target_link_libraries(${TARGET_NAME} mongo::mongocxx_shared)
endfunction()

# used to define a bitxorcore mongo int test executable by combining bitxorcore_test_executable and bitxorcore_target
function(bitxorcore_int_test_executable_target TARGET_NAME)
	bitxorcore_test_executable(${TARGET_NAME} ${ARGN})
	bitxorcore_target(${TARGET_NAME})

	set_property(TEST ${TARGET_NAME} PROPERTY LABELS Database)

	# customize and export compiler options for gtest
	bitxorcore_set_test_compiler_options()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()

add_subdirectory(plugins)

bitxorcore_define_extension_no_group(mongo)
bitxorcore_add_mongo_dependencies(extension.mongo)

# add extension to mongo group
add_dependencies(mongo extension.mongo)
