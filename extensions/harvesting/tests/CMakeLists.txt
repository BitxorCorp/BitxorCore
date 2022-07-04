cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_test(harvesting test)
target_link_libraries(tests.bitxorcore.harvesting tests.bitxorcore.test.crypto)
