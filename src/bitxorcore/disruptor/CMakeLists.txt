cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.disruptor)
target_link_libraries(bitxorcore.disruptor bitxorcore.model)
