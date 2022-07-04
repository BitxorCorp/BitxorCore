cmake_minimum_required(VERSION 3.14)

add_definitions(-DBITXORCORE_VERSION_DESCRIPTION="${BITXORCORE_VERSION_DESCRIPTION}")
bitxorcore_library_target(bitxorcore.version)

if(NOT MSVC)
	add_subdirectory(nix)
endif()
