cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_src(finalization api chain handlers io ionet model)
target_link_libraries(bitxorcore.finalization bitxorcore.consumers bitxorcore.crypto_voting)
