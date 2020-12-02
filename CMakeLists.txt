cmake_minimum_required(VERSION 2.8)

project(websocket_c)

set(EXE_DIR build/)
SET(SOURCE_DIR src/)
SET(HEADER_DIR include/)

set(SSL_CONNECTION 1)

# config for program
configure_file(
	        "${PROJECT_SOURCE_DIR}/cmake/config.h.in"
			"${SOURCE_DIR}/config.h"
)

set(websocket_SOURCE
	${SOURCE_DIR}helper.c
	${SOURCE_DIR}generator.c
	${SOURCE_DIR}websocket.c
)
set(websocket_HEADER
	${HEADER_DIR}helper.h
	${HEADER_DIR}generator.h
	${HEADER_DIR}websocket.h
)

include_directories(${HEADER_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${EXE_DIR})
SET(LIBRARY_OUTPUT_PATH ${EXE_DIR}lib)

add_library(websocket SHARED ${websocket_HEADER} ${websocket_SOURCE})

find_package (Threads)
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(websocket
	${CMAKE_THREAD_LIBS_INIT}
	${OPENSSL_LIBRARIES})

#Tests
set(TEST_WEBSOCKET tests/main.c)
add_executable(websocket_test tests/main.c)
target_link_libraries(websocket_test websocket)
