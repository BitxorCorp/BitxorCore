cmake_minimum_required(VERSION 3.14)
project(external)
add_definitions(-DNO_MISALIGNED_ACCESSES) # sha3

file(GLOB external_donna_SRC
	"donna/*.c"
	"donna/*.h"
)

SOURCE_GROUP("donna" FILES ${external_donna_SRC})

add_library(external
	${external_donna_SRC}
	README.txt)

bitxorcore_add_openssl_dependencies(external)

set_property(TARGET external PROPERTY POSITION_INDEPENDENT_CODE ON)
