cmake_minimum_required (VERSION 2.8)

project(cmakeDemo1)

set(SRC_LIST main.cpp)

message(STATUS "this is BINARY dir:" ${PROJECT_BINARY_DIR})
message(STATUS "this is SOURCE dir:" ${PROJECT_SOURCE_DIR})
message(STATUS "this is CMake  dir:" ${CMAKE_SOURCE_DIR})

option(SET_DEBUG "set SET_DEBUG" ON)
message(STATUS "SET_DEBUG option is ${SET_DEBUG}")

if(SET_DEBUG)
    message(STATUS "defined SET_DEBUG")
	add_definitions(-DSET_DEBUG)            #添加宏定义#define SET_DEBUG
else()
    message(STATUS "NOT defined SET_DEBUG")
	add_definitions(-DSET_DEBUG_OFF)        #添加宏定义#define SET_DEBUG_OFF
endif()
add_executable(cmakeDemo ${SRC_LIST})


