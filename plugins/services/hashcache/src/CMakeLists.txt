cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.plugins.hashcache)

# create a cache lib
bitxorcore_library_target(${PLUGIN_BASE_NAME}.cache cache)
target_link_libraries(${PLUGIN_BASE_NAME}.cache bitxorcore.cache bitxorcore.state)

# create a deps lib
bitxorcore_library_target(${PLUGIN_BASE_NAME}.deps handlers observers validators)
target_link_libraries(${PLUGIN_BASE_NAME}.deps ${PLUGIN_BITXORCORE_LIBS} ${PLUGIN_BASE_NAME}.cache)

# create a plugin dll
bitxorcore_shared_library_target(${PLUGIN_BASE_NAME} plugins)
target_link_libraries(${PLUGIN_BASE_NAME} ${PLUGIN_BASE_NAME}.deps)

add_dependencies(plugins ${PLUGIN_BASE_NAME})
