cmake_minimum_required(VERSION 3.5)

project(pbio_cpp)

add_library(pbio_cpp STATIC noop.cpp)

target_include_directories(pbio_cpp PUBLIC ../../../include)

if(TARGET_OS_LINUX)
	target_link_libraries(pbio_cpp dl)
endif()