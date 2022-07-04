cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(tests.bitxorcore.int.node.test)
target_link_libraries(tests.bitxorcore.int.node.test bitxorcore.plugins.coresystem tests.bitxorcore.test.local)
