cmake_minimum_required(VERSION 3.14)

set(PLUGIN_BASE_NAME bitxorcore.mongo.plugins.locksecret)

include_directories(${PROJECT_SOURCE_DIR}/plugins/txes/lock_secret)
bitxorcore_mongo_plugin_src_with_deps(${PLUGIN_BASE_NAME} mappers storages)
