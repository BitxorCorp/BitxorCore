cmake_minimum_required(VERSION 3.14)

message("--- locating zeromq dependencies ---")
find_package(cppzmq 4.7.1 EXACT REQUIRED)

message("zeromq    ver: ${ZeroMQ_VERSION}")
message("zeromq    inc: ${ZeroMQ_INCLUDE_DIR}")
message("zeromq    lib: ${ZeroMQ_LIBRARY}")
message("cppzmq    ver: ${cppzmq_VERSION}")
message("cppzmq    inc: ${cppzmq_INCLUDE_DIR}")

### find and set zeromq includes and libraries
function(bitxorcore_add_zeromq_dependencies TARGET_NAME)
	target_link_libraries(${TARGET_NAME} cppzmq)
endfunction()

bitxorcore_define_extension(zeromq)
bitxorcore_add_zeromq_dependencies(extension.zeromq)
target_link_libraries(extension.zeromq bitxorcore.extensions)
