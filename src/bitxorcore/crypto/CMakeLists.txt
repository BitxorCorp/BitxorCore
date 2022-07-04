cmake_minimum_required(VERSION 3.14)

include_directories(../../../external)

bitxorcore_library_target(bitxorcore.crypto)

target_link_libraries(bitxorcore.crypto bitxorcore.utils external)
bitxorcore_add_openssl_dependencies(bitxorcore.crypto)
