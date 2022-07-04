cmake_minimum_required(VERSION 3.14)

bitxorcore_test_executable_target_header_only(tests.bitxorcore.keylink cache)
target_link_libraries(tests.bitxorcore.keylink bitxorcore.observers bitxorcore.validators)
