cmake_minimum_required(VERSION 3.14)

bitxorcore_define_tool(votingkey)
target_link_libraries(bitxorcore.tools.votingkey bitxorcore.crypto_voting bitxorcore.finalization)
