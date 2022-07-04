cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME tests.bitxorcore.plugins.transfer)

bitxorcore_tx_plugin_tests(${TARGET_NAME})
target_link_libraries(tests.bitxorcore.plugins.transfer tests.bitxorcore.test.net)
