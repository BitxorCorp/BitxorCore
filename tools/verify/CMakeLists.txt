cmake_minimum_required(VERSION 3.14)

bitxorcore_define_tool(verify)
target_link_libraries(bitxorcore.tools.verify bitxorcore.tools.plugins bitxorcore.finalization)
