cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.int.stress)

bitxorcore_int_test_executable_target(${TARGET_NAME} test)
target_link_libraries(${TARGET_NAME}
	bitxorcore.cache_db
	bitxorcore.harvesting
	bitxorcore.plugins.hashcache
	bitxorcore.plugins.hashcache.cache
	tests.bitxorcore.test.local
	tests.bitxorcore.test.genesis)

set_property(TEST ${TARGET_NAME} PROPERTY LABELS Stress)

# add dependency on hash cache plugin
include_directories(../../../plugins/services/hashcache)
