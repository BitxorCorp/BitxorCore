cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.restrictiontoken)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/restriction_token)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)

# restrictiontoken.deps is needed because the mongo token restriction cache storage needs the token restriction cache
target_link_libraries(${PLUGIN_BASE_NAME}.deps bitxorcore.plugins.restrictiontoken.deps)
