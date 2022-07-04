cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.test.core mocks)
target_link_libraries(tests.bitxorcore.test.core
	tests.bitxorcore.test.nodeps
	${CORE_BITXORCORE_LIBS}
	bitxorcore.sdk)
