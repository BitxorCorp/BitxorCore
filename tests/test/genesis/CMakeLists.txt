cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.genesis)
target_link_libraries(tests.bitxorcore.test.genesis
	tests.bitxorcore.test.local
	bitxorcore.plugins.token
	bitxorcore.plugins.namespace
	bitxorcore.plugins.transfer) # shared lib dependencies allow tests to create real transaction plugins

include_directories(../../../plugins/txes/token)
include_directories(../../../plugins/txes/namespace)
