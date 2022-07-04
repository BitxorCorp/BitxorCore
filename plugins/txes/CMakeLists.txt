cmake_minimum_required(VERSION 3.14)

function(bitxorcore_tx_plugin_src PLUGIN_BASE_NAME)
	# create an sdk lib
	if(DEFINED PLUGIN_SDK_FOLDERS)
		bitxorcore_library_target(${PLUGIN_BASE_NAME}.sdk ${PLUGIN_SDK_FOLDERS})
		target_link_libraries(${PLUGIN_BASE_NAME}.sdk bitxorcore.model)
	endif()

	# create a deps lib
	if(DEFINED PLUGIN_DEPS_FOLDERS)
		bitxorcore_library_target(${PLUGIN_BASE_NAME}.deps ${PLUGIN_DEPS_FOLDERS})
		target_link_libraries(${PLUGIN_BASE_NAME}.deps ${PLUGIN_BITXORCORE_LIBS})
	endif()

	if(DEFINED PLUGIN_SDK_FOLDERS)
		target_link_libraries(${PLUGIN_BASE_NAME}.deps ${PLUGIN_BASE_NAME}.sdk)
	endif()

	# create a plugin dll
	bitxorcore_shared_library_target(${PLUGIN_BASE_NAME} plugins)
	target_link_libraries(${PLUGIN_BASE_NAME} ${PLUGIN_BITXORCORE_LIBS})

	if(DEFINED PLUGIN_DEPS_FOLDERS)
		target_link_libraries(${PLUGIN_BASE_NAME} ${PLUGIN_BASE_NAME}.deps)
	endif()

	add_dependencies(plugins ${PLUGIN_BASE_NAME})
endfunction()

function(bitxorcore_tx_plugin_tests TARGET_NAME)
	string(REGEX MATCH "\.[a-z_]+$" PLUGIN_NAME ${TARGET_NAME})
	string(SUBSTRING ${PLUGIN_NAME} 1 -1 PLUGIN_NAME)

	if(NOT TARGET "tests.bitxorcore.test.plugins.${PLUGIN_NAME}")
		set(PLUGIN_TEST_FOLDERS test ${PLUGIN_TEST_FOLDERS})
	endif()

	bitxorcore_test_executable_target(${TARGET_NAME} cache ${PLUGIN_SDK_FOLDERS} ${PLUGIN_DEPS_FOLDERS} ${PLUGIN_TEST_FOLDERS} plugins)

	if(TARGET "tests.bitxorcore.test.plugins.${PLUGIN_NAME}")
		target_link_libraries(${TARGET_NAME} tests.bitxorcore.test.plugins.${PLUGIN_NAME})
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()

function(bitxorcore_plugin_test_library TARGET_NAME)
	bitxorcore_library_target(${TARGET_NAME})
	target_link_libraries(${TARGET_NAME} ${GTEST_LIBRARIES})
endfunction()

add_subdirectory(account_link)
add_subdirectory(aggregate)
add_subdirectory(lock_hash)
add_subdirectory(lock_secret)
add_subdirectory(lock_shared)
add_subdirectory(metadata)
add_subdirectory(token)
add_subdirectory(multisig)
add_subdirectory(namespace)
add_subdirectory(restriction_account)
add_subdirectory(restriction_token)
add_subdirectory(transfer)
