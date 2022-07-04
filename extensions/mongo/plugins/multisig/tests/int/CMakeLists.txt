cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.int.mongo.plugins.multisig)

bitxorcore_int_test_executable_target(${TARGET_NAME} storages)
bitxorcore_add_mongo_dependencies(${TARGET_NAME})
target_link_libraries(${TARGET_NAME}
	tests.bitxorcore.test.mongo.plugins.multisig
	tests.bitxorcore.test.cache
	tests.bitxorcore.test.mongo
	bitxorcore.mongo.plugins.multisig
	bitxorcore.plugins.multisig.deps)
