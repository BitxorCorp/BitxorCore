cmake_minimum_required(VERSION 3.14)

bitxorcore_library_target(bitxorcore.chain)
target_link_libraries(bitxorcore.chain
	bitxorcore.api
	bitxorcore.cache_core
	bitxorcore.cache_tx
	bitxorcore.disruptor
	bitxorcore.model
	bitxorcore.observers
	bitxorcore.utils
	bitxorcore.validators)
