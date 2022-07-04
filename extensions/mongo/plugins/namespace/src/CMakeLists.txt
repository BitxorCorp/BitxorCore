cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.namespace)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/namespace)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)

# namespace.deps is needed because the mongo namespace cache storage needs the namespace cache
target_link_libraries(${PLUGIN_BASE_NAME}.deps bitxorcore.plugins.namespace.deps)
