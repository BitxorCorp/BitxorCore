cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.extensions)
target_link_libraries(bitxorcore.extensions bitxorcore.chain bitxorcore.net bitxorcore.plugins bitxorcore.subscribers)
