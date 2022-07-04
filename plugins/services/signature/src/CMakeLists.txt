cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.plugins.signature)

# create a deps lib
bitxorcore_library_target(${PLUGIN_BASE_NAME}.deps validators)
target_link_libraries(${PLUGIN_BASE_NAME}.deps ${PLUGIN_BITXORCORE_LIBS})

# create a plugin dll
bitxorcore_shared_library_target(${PLUGIN_BASE_NAME} plugins)
target_link_libraries(${PLUGIN_BASE_NAME} ${PLUGIN_BASE_NAME}.deps)

add_dependencies(plugins ${PLUGIN_BASE_NAME})
