cmake_minimum_required(VERSION 3.14)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

set(PLUGIN_BITXORCORE_LIBS
	${CORE_BITXORCORE_LIBS}
	bitxorcore.cache
	bitxorcore.cache_core
	bitxorcore.config
	bitxorcore.observers
	bitxorcore.plugins
	bitxorcore.state
	bitxorcore.validators)

add_subdirectory(coresystem)
add_subdirectory(services)
add_subdirectory(txes)
