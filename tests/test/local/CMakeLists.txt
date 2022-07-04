cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.local)
target_link_libraries(tests.bitxorcore.test.local
	tests.bitxorcore.test.cache
	tests.bitxorcore.test.genesis
	tests.bitxorcore.test.net
	bitxorcore.local.server)
