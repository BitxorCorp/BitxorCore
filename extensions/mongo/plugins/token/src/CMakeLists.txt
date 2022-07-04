cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.token)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/token)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)

# token.deps is needed because the mongo token cache storage needs the token cache
target_link_libraries(${PLUGIN_BASE_NAME}.deps bitxorcore.plugins.token.deps)
