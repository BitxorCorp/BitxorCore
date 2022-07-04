cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_src(syncsource)
target_link_libraries(bitxorcore.syncsource bitxorcore.io)
