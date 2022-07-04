cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.restrictionaccount)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/restriction_account)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)

# restrictionaccount.deps is needed because the mongo account restriction cache storage needs the account restriction cache
target_link_libraries(${PLUGIN_BASE_NAME}.deps bitxorcore.plugins.restrictionaccount.deps)
