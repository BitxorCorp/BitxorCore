cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.sdk)

bitxorcore_library_target(${TARGET_NAME} builders extensions parsers)
target_link_libraries(${TARGET_NAME}
	bitxorcore.model
	bitxorcore.state
	bitxorcore.plugins.aggregate.sdk
	bitxorcore.plugins.metadata.sdk
	bitxorcore.plugins.token.sdk
	bitxorcore.plugins.namespace.sdk
	bitxorcore.plugins.restrictionaccount.sdk)

find_program(PYTHON_EXECUTABLE python3 python)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/bitxorcore/bitxorcore.h
	DEPENDS ${PROJECT_SOURCE_DIR}/scripts/sdk/publishSdk.py
	COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/sdk/publishSdk.py ARGS --r ${PROJECT_SOURCE_DIR} --p ${CMAKE_BINARY_DIR}/inc)

add_custom_target(
	publish
	DEPENDS ${CMAKE_BINARY_DIR}/bitxorcore/bitxorcore.h)
