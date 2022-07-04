cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target(tests.bitxorcore.crypto_voting core test)
target_link_libraries(tests.bitxorcore.crypto_voting tests.bitxorcore.test.crypto)
