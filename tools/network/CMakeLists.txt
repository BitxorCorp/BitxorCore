cmake_minimum_required(VERSION 3.14)

bitxorcore_define_tool(network)
target_link_libraries(bitxorcore.tools.network bitxorcore.nodediscovery)
