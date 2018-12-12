cmake_minimum_required (VERSION 3.5)

set(TARGET iso_8583_c)

set(PROJ_PATH ${CMAKE_SOURCE_DIR})

project({TARGET})

include_directories(${PROJ_PATH}/inc)

set(EXECUTABLE_OUTPUT_PATH ${PROJ_PATH}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE
	${PROJ_PATH}/main.c
)

add_executable(${TARGET} ${SOURCE})
