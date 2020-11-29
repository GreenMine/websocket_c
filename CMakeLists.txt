cmake_minimum_required(VERSION 2.8)

project(websocket_c)

set(EXE_DIR build/)
SET(SOURCE_DIR src/)
set(SOURCE_EXE ${SOURCE_DIR}main.c)
set(websocket_SOURCE_DIR ${SOURCE_DIR}websocket/)

set(websocket_SOURCE
	${websocket_SOURCE_DIR}helper.c
	${websocket_SOURCE_DIR}generator.c
	${websocket_SOURCE_DIR}websocket.c
)
set(websocket_HEADER
	${websocket_SOURCE_DIR}helper.h
	${websocket_SOURCE_DIR}generator.h
	${websocket_SOURCE_DIR}websocket.h
	)
SET(EXECUTABLE_OUTPUT_PATH ${EXE_DIR})
SET(LIBRARY_OUTPUT_PATH ${EXE_DIR}lib)

add_library(websocket STATIC
	${websocket_SOURCE}
	${websocket_HEADER}
	)

add_executable(main ${SOURCE_EXE})

find_package (Threads)

find_package(OpenSSL REQUIRED)
#message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
#message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")
include_directories(${OPENSSL_INCLUDE_DIR})

target_link_libraries(websocket
	${CMAKE_THREAD_LIBS_INIT}
	${OPENSSL_LIBRARIES})
target_link_libraries(main websocket)
