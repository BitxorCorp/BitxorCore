cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.ionet)
target_link_libraries(bitxorcore.ionet bitxorcore.model bitxorcore.thread)
bitxorcore_add_openssl_dependencies(bitxorcore.ionet)
