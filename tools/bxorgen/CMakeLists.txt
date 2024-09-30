cmake_minimum_required(VERSION 3.14)

add_subdirectory(blockhashes)

bitxorcore_define_tool(bxorgen)
target_link_libraries(bitxorcore.tools.bxorgen
	bitxorcore.tools.bxorgen.blockhashes
	bitxorcore.tools.plugins
	bitxorcore.finalization
	bitxorcore.plugins.coresystem
	bitxorcore.plugins.token
	bitxorcore.plugins.namespace
	bitxorcore.plugins.transfer)
