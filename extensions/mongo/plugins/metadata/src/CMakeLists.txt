cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.metadata)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/metadata)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)

# metadata.deps is needed because the mongo metadata cache storage needs the metadata cache
target_link_libraries(${PLUGIN_BASE_NAME}.deps bitxorcore.plugins.metadata.deps)
