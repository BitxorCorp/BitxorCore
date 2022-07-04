cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_src(partialtransaction api chain handlers)
target_link_libraries(bitxorcore.partialtransaction bitxorcore.consumers bitxorcore.plugins.aggregate.sdk)
