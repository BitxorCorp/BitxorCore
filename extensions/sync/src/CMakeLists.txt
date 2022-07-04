cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_src(sync)
target_link_libraries(bitxorcore.sync bitxorcore.consumers)
